#include <stdio.h>

#include "retrier.h"

int add(int a, int b)
{
	return a + b;
}

fn_retry_status add_wrapper(void *args)
{
	struct {
		int a;
		int b;
	} *fn_args;

	fn_args = args;

	static volatile int cnt = 3;

	if (cnt != 0) {
		cnt--;
		return (fn_retry_status)
		{
			.err = "not yet",
				.ok = 0,
				.ret = NULL,
				.retry = 1
		};
	}

	// use malloc for stable results
	volatile int res = add(fn_args->a, fn_args->b);

	return (fn_retry_status)
	{
		.err = "OK",
			.ok = 1,
			.ret = &res,
			.retry = 0
	};
}

int main(void)
{
	retrier_t retrier = new_basic_retrier("log.txt");
	// retrier.log = stdout;

	retry_result res = retry_fn(add_wrapper, &(struct { int a;  int b }) {
		.a = 2, .b = 3
	}, &retrier);

	int r = *((int *)res.final_status.ret);
	printf("%s: %d\nRetry cnt: %d\n", res.err, r, res.retry_cnt);

	return 0;
}
