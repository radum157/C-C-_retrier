#ifndef __RETRIER_H
#define __RETRIER_H 1

#include <stdio.h>

// Function retrier
typedef struct retrier_t {
	// max retry count
	unsigned int cnt;
	// should be ns
	unsigned int max_sleep_time;
	FILE *log;
} retrier_t;

typedef struct fn_retry_status {
	char ok;
	// return value
	void *ret;
	// if retry is possible
	char retry;
	char err[100];
} fn_retry_status;

typedef struct retry_result {
	char err[100];
	fn_retry_status final_status;
	unsigned int retry_cnt;
} retry_result;

// NULL log file in retrier @a cfg means @a fopen has failed
retrier_t new_basic_retrier(const char *log_file);
// Default values: cnt = 5, max_sleep_time = 200000
retrier_t new_default_retrier(void);

/**
 * @param fn function that returns fn_retry_status. In case of multiple arguments, use structures to encapsulate and pass them to @a args
*/
retry_result retry_fn(fn_retry_status(*fn)(void *), void *args, retrier_t *retrier);

// note that this calls fclose on cfg.log, ergo incompatible with stdout logging
int destroy_retrier(retrier_t *retrier);

#endif
