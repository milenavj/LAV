// Project headers
#include "lav/Threads/SignalingThread.h"

// STL headers
#include <utility>

using namespace std;
using namespace Utils;


SignalingThreadBaseFromMember::SignalingThreadBaseFromMember()
    :m_event{Event::Create()}
{}

SignalingThreadBaseFromMember::SignalingThreadBaseFromMember(SignalingThreadBaseFromMember &&st)
    :m_event{move(st.m_event)}
{}

SignalingThreadBaseFromMember& SignalingThreadBaseFromMember::operator=(SignalingThreadBaseFromMember &&st)
{
    if(this != &st)
    {
        m_event = move(st.m_event);
    }
    return *this;
}

SignalingThreadBaseFromMember::~SignalingThreadBaseFromMember()
{}


SignalingThread::SignalingThread(Callable f)
    :SignalingThreadBaseFromMember{}, CancelableThread{bind(f,m_event)}
{}

SignalingThread::SignalingThread(SignalingThread &&st)
    :SignalingThreadBaseFromMember{move(st)},CancelableThread{move(st)}
{}

SignalingThread& SignalingThread::operator=(SignalingThread &&st)
{
    if(this != &st)
    {
       SignalingThreadBaseFromMember::operator =(move(st));
       CancelableThread::operator =(move(st));
    }
    return *this;
}
SignalingThread::~SignalingThread()
{}

const Event::Pointer& SignalingThread::ShareEvent() const
{
    return m_event;
}
