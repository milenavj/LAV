#ifndef FUTURERESULT_H
#define FUTURERESULT_H

#include <string>
#include <future>

namespace Threads
{
	class FutureResult
	{
	public:
		enum ExecStatus
		{
			Pending = 1,
			Executing = 2,
			Finished = 3
		};

	private:
		ExecStatus m_exec_status;
		std::promise<int> m_promise;
		std::shared_future<int> m_shared_future;

	public:
		FutureResult();
		FutureResult(FutureResult &&fr);
		FutureResult& operator=(FutureResult &&ft);
		~FutureResult();

		void setResult(int val);
		std::shared_future<int>& getSharedFuture();
		ExecStatus getStatus();
		void setStatus(ExecStatus es);
		

	};
}

#endif // FUTURERESULT_H
