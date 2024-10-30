#include <time.h>

#include "retrier.h"

retrier_t new_basic_retrier(const char *log_file)
{
	retrier_t retrier = new_default_retrier();
	if (log_file == NULL) {
		return retrier;
	}

	retrier.log = fopen(log_file, "at");
	return retrier;
}

retrier_t new_default_retrier(void)
{
	return (retrier_t)
	{
		.cnt = 5,
			.max_sleep_time = 1000000000,
			.log = NULL
	};
}

int destroy_retrier(retrier_t *retrier)
{
	if (retrier->log) {
		return fclose(retrier->log);
	}
	return 0;
}

retry_result retry_fn(fn_retry_status(*fn)(void *), void *args, retrier_t *retrier)
{
	unsigned int ns = 100;
	fn_retry_status status;

	for (unsigned int i = 0; i < retrier->cnt; i++) {
		status = fn(args);

		if (status.ok) {
			return (retry_result)
			{
				.err = "OK",
					.final_status = status,
					.retry_cnt = i + 1
			};
		}

		if (!status.retry) {
			return (retry_result)
			{
				.err = "Retry not recommended by function",
					.final_status = status,
					.retry_cnt = i + 1
			};
		}

		if (retrier->log != NULL) {
			// Make it thread-safe
			flockfile(retrier->log);
			fprintf(retrier->log, "%s\n", status.err);
			funlockfile(retrier->log);
		}

		// Wait before retry
		nanosleep(&(struct timespec)
		{
			.tv_nsec = ns,
				.tv_sec = 0
		}, NULL);

		if (ns <= (retrier->max_sleep_time >> 1)) {
			ns <<= 1;
		}
	}

	return (retry_result)
	{
		.err = "Exceeded maximum number of retries",
			.final_status = status,
			.retry_cnt = retrier->cnt
	};
}
