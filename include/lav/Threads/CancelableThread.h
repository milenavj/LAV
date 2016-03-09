#ifndef CANCELABLETHREAD_H
#define CANCELABLETHREAD_H

// STL headers
#include <thread>
#include <functional>

namespace  Utils
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

        void Cancel();
        bool IsJoinable() const;
        void Join();

    private:
        static void AllowCancel();
    };
}

#endif // CANCELABLETHREAD_H
