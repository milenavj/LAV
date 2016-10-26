#ifndef CANCELABLETHREAD_H
#define CANCELABLETHREAD_H

#include <thread>
#include <functional>

namespace Threads
{
	class CancelableThread
	{
	public:
		using Callable = std::function<void()>;

	private:
		std::thread m_worker;
		bool m_finished;

	public:
		CancelableThread(Callable f);
		CancelableThread(CancelableThread &&ct);
		CancelableThread& operator=(CancelableThread &&ct);
		virtual ~CancelableThread();

		// Funkcija za cancel niti
		void Cancel();
        std::thread& GetHandle();

	private:
		static void AllowCancel();
	};	
}

#endif //CANCELABLETHREAD_H
