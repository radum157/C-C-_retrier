#ifndef __RETRIER_HPP
#define __RETRIER_HPP 1

#include <string>
#include <time.h>
#include <fstream>
#include <mutex>

template<typename T>
struct RetryFuncResult {
	bool ok;
	std::string err;
	bool retry;
	T res;
};

template<typename T>
struct RetryResult {
	std::string msg;
	RetryFuncResult<T> final_state;
	unsigned int retry_cnt;
};

class Retrier {
public:

	class Config {
	public:
		unsigned int cnt;
		long max_sleep_time;
		std::ostream *log;

		Config()
		{
			log = nullptr;
			cnt = 5;
			max_sleep_time = 1000000000;
		}
		Config(std::ostream &os)
		{
			log = &os;
		}
		Config(const Config &cfg)
		{
			log = cfg.log;
			cnt = cfg.cnt;
			max_sleep_time = cfg.max_sleep_time;
		}
	};

	Config cfg;

	std::mutex mux;

	Retrier() {}
	Retrier(const Config &cfg) : cfg(cfg) {}
	~Retrier() {}

	template<typename T, typename ...Ts>
	RetryResult<T> retry_fn(RetryFuncResult<T>(*fn)(Ts...), Ts... args)
	{
		long ns = 100;
		RetryFuncResult<T> res;

		for (unsigned int i = 0; i < cfg.cnt; i++) {
			res = fn(args...);

			if (res.ok) {
				return (RetryResult<T>)
				{
					"OK",
						res,
						i + 1
				};
			}

			if (!res.retry) {
				return (RetryResult<T>)
				{
					"Retry not recommended by function",
						res,
						i + 1
				};
			}

			if (cfg.log != nullptr) {
				std::lock_guard<std::mutex> lock(mux);
				(*cfg.log) << res.err << '\n';
			}

			if (ns <= (cfg.max_sleep_time >> 1)) {
				ns <<= 1;
			}

			struct timespec tmp = {
				0,
				ns
			};
			nanosleep(&tmp, NULL);
		}

		return (RetryResult<T>)
		{
			"Exceeded maximum retry count",
				res,
				cfg.cnt
		};
	}
};

#endif
