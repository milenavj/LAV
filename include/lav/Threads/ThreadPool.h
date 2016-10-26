#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "CancelableThread.h"
#include "Event.h"
#include "SignalingThread.h"
#include "FixedQueue.h"

// STL zaglavlja
#include <thread>
#include <vector>
#include <functional>
#include <memory>

namespace Threads
{
	class ThreadPool
	{
	public:
		enum Result{
			Succ = 1,
			Unsucc = 2
		};
		
		ThreadPool();
		ThreadPool(std::vector<std::function<int()>> &&tasks, uint64_t num_threads = std::thread::hardware_concurrency() -1);
		ThreadPool(ThreadPool &&tp);
		ThreadPool& operator=(ThreadPool &&oth);
		~ThreadPool();

		void Init(std::vector<std::function<int()>> &&tasks, uint64_t num_threads = std::thread::hardware_concurrency() -1);
		void Work();
		void StartWorkerThreads();
		void StartControlThread();
		void JoinWorkerThreads();
		void JoinControlThread();
		void ShutDown();
		Result GetResult();
	private:
		std::shared_ptr<FixedQueue<std::function<int()>>> m_tasksPtr;
		std::vector<SignalingThread> m_threads;
        std::thread m_control_thread;
		uint64_t m_num_threads;
		uint64_t m_num_tasks;
		Result m_result;
	};
}


#endif //THREADPOOL_H
