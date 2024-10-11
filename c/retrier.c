#include <time.h>

#include "retrier.h"

retrier_t new_basic_retrier(const char* log_file) {
    retrier_t retrier = new_default_retrier();

    retrier.cfg.log = fopen(log_file, "at");
    if(retrier.cfg.log == NULL) {
        return retrier;
    }

    int rc = pthread_mutex_init(&retrier.cfg.mux, NULL);
    if(rc < 0) {
        retrier.cfg.log = NULL;
    }

    return retrier;
}

retrier_t new_retrier(const retrier_cfg* cfg) {
    return (retrier_t) {
        .cfg = *cfg
    };
}

retrier_t new_default_retrier(void) {
    return (retrier_t) {
        .cfg = (retrier_cfg){
            .cnt = 5,
            .max_sleep_time = 1000000000,
            .log = NULL
        }
    };
}

int destroy_retrier(retrier_t* retrier) {
    if(retrier->cfg.log) {
        fclose(retrier->cfg.log);
        return pthread_mutex_destroy(&retrier->cfg.mux);
    }
    return 0;
}

retry_result retry_fn(fn_retry_status(*fn)(void*), void* args, retrier_t* retrier) {
    unsigned int ns = 100;
    fn_retry_status status;

    for(unsigned int i = 0; i < retrier->cfg.cnt; i++) {
        status = fn(args);

        if(status.ok) {
            return (retry_result) {
                .err = "OK",
                    .final_status = status,
                    .retry_cnt = i + 1
            };
        }

        if(!status.retry) {
            return (retry_result) {
                .err = "Retry not recommended by function",
                    .final_status = status,
                    .retry_cnt = i + 1
            };
        }

        if(retrier->cfg.log != NULL) {
            pthread_mutex_lock(&retrier->cfg.mux);
            fprintf(retrier->cfg.log, "%s\n", status.err);
            pthread_mutex_unlock(&retrier->cfg.mux);
        }

        // wait before retry
        nanosleep(&(struct timespec) {
            .tv_nsec = ns,
                .tv_sec = 0
        }, NULL);

        if(ns <= (retrier->cfg.max_sleep_time >> 1)) {
            ns <<= 1;
        }
    }

    return (retry_result) {
        .err = "Exceeded maximum number of retries",
            .final_status = status,
            .retry_cnt = retrier->cfg.cnt
    };
}
