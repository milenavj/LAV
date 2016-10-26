#include "lav/Threads/SignalingThread.h"

// STL zaglavlja
#include <utility>

using namespace Threads;

SignalingThreadBaseFromMember::SignalingThreadBaseFromMember()
	:m_event{Event::Create()}
	{}

SignalingThreadBaseFromMember::SignalingThreadBaseFromMember(SignalingThreadBaseFromMember && st)
	:m_event{std::move(st.m_event)}
	{}

SignalingThreadBaseFromMember& SignalingThreadBaseFromMember::operator=(SignalingThreadBaseFromMember &&st)
	{
		if(this != &st)
		{
			m_event = std::move(st.m_event);	
		}
		return *this;
	}
SignalingThreadBaseFromMember::~SignalingThreadBaseFromMember(){}

SignalingThread::SignalingThread(Callable f)
	:SignalingThreadBaseFromMember{}, CancelableThread{std::bind(f, m_event)}
	{}


SignalingThread::SignalingThread(SignalingThread &&st)
	:SignalingThreadBaseFromMember{std::move(st)}, CancelableThread{std::move(st)}
	{}


SignalingThread& SignalingThread::operator=(SignalingThread &&st)
	{
		if(this != &st)
		{
			SignalingThreadBaseFromMember::operator=(std::move(st));
			CancelableThread::operator=(std::move(st));
		}
		return *this;
	}

SignalingThread::~SignalingThread(){}

const Event::Pointer& SignalingThread::ShareEvent() const 
	{
 		return m_event;
   	}
