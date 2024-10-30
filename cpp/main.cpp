#include <iostream>

#include "retrier.hpp"

int add(int a, int b)
{
	return a + b;
}

RetryFuncResult<int> add_wrapper(int a, int b)
{
	static volatile int cnt = 3;

	if (cnt == 0) {
		return (RetryFuncResult<int>)
		{
			.ok = true,
				.err = "OK",
				.retry = false,
				.res = a + b
		};
	}

	cnt--;

	return (RetryFuncResult<int>)
	{
		.ok = false,
			.err = "Try again",
			.retry = true,
			.res = 0
	};
}

int main()
{
	Retrier retrier(Retrier::Config(std::cout));

	auto res = retrier.retry_fn(add_wrapper, 2, 3);

	std::cout << res.msg << ": " << res.final_state.res <<
		"\nRetry count: " << res.retry_cnt << '\n';

	return 0;
}
