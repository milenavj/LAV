#include "lav/Threads/CancelableThread.h"

// STL zaglavlja
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <string>
#include <iostream>

// Ostala zaglavlja
#include <pthread.h>

using namespace Threads;

CancelableThread::CancelableThread(Callable f)
	:m_worker{
		[=] () {
			AllowCancel();
			f();
		}
	},
	m_finished{false}
{}

CancelableThread::CancelableThread(CancelableThread &&ct)
	:m_worker{std::move(ct.m_worker)}, m_finished{ct.m_finished}
{}

CancelableThread& CancelableThread::operator=(CancelableThread &&ct)
{
	if(&ct != this)
	{
		m_worker = std::move(ct.m_worker);
		m_finished = ct.m_finished;
	}
	return *this;
}

CancelableThread::~CancelableThread() { }

std::thread& CancelableThread::GetHandle()
{
    return m_worker;
}

void CancelableThread::Cancel()
{
	// Ukoliko nit nije zavrsila mozemo je cancel-ovati
	if(!m_finished)
	{
		// Uzimamo sistemski id niti
		pthread_t tid = m_worker.native_handle();

		// Ukoliko mozemo da nadjemo tu nit, otkazujemo je
		// inace ukoliko ne postoji id to znaci da se zavrsila
		if(tid)
		{
std::cout << "OTKAZUJEMO NIT " << tid<<std::endl;
			int pthreadErrno;

			if((pthreadErrno = pthread_cancel(tid)) != 0 && pthreadErrno != ESRCH)
			{
				throw std::runtime_error{ std::string{"Threads::Greska prilikom cancel-ovanja niti. --- "} + ::strerror(pthreadErrno)};

			}
		}

		m_finished = true;
	}
}


void  CancelableThread::AllowCancel()
{
	// Dozvoljavamo niti da bude cancel-ovana
	if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr) != 0)
		throw std::runtime_error{ std::string{"Threads::Greska, nismo uspeli da dozvolimo niti da bude cancel-ovana. --- "} + ::strerror(errno)};

	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr) != 0)
		throw std::runtime_error{ std::string{"Threads::Greska, nismo uspeli da dozvolimo niti da bude cancel-ovana. --- "} + ::strerror(errno)};

}
