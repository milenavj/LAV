#ifndef SIGNALINGTHREAD_H
#define SIGNALINGTHREAD_H

// Project headers
#include "CancelableThread.h"
#include "Event.h"

// STL headers
#include <functional>

namespace Utils
{
    /*
     * This struct is neccessary for initialization of Event::Pointer m_event field
     * of SignalingThread class (its now moved into this struct) to occure before initialization of CancelableThread
     * which is base for SignalingThread.
    */
    struct SignalingThreadBaseFromMember
    {
        Event::Pointer m_event;
        explicit SignalingThreadBaseFromMember();
        SignalingThreadBaseFromMember(SignalingThreadBaseFromMember &&st);
        SignalingThreadBaseFromMember& operator=(SignalingThreadBaseFromMember &&st);
        ~SignalingThreadBaseFromMember();
    };

    class SignalingThread : protected SignalingThreadBaseFromMember, public CancelableThread
    {
    public:
        using Callable = std::function<void(const Event::Pointer&)>;

        SignalingThread(Callable f);
        SignalingThread(SignalingThread &&st);
        SignalingThread& operator=(SignalingThread &&st);
        virtual ~SignalingThread();

        // Sharing event which controlling thread will wait for.
        const Event::Pointer& ShareEvent() const;

    };

}

#endif // SIGNALINGTHREAD_H
