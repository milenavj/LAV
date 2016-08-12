#include "lav/Threads/CancelableThread.h"

// STL headers
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <string>

#include <pthread.h>

using namespace std;
using namespace Utils;

CancelableThread::CancelableThread(Callable f) : m_worker {
  [ = ]() {
    AllowCancel();
    f();
  }
}
, m_finished { false }
{}

CancelableThread::CancelableThread(CancelableThread &&ct) : m_worker {
  move(ct.m_worker)
}
, m_finished { ct.m_finished }
{}

CancelableThread &CancelableThread::operator=(CancelableThread &&ct) {
  if (&ct != this) {
    m_worker = move(ct.m_worker);
    m_finished = ct.m_finished;
  }
  return *this;
}

CancelableThread::~CancelableThread() { Cancel(); }

void CancelableThread::Cancel() {
// Check if we can use POSIX threads
#ifndef __unix__
  static_assert(false, "POSIX threads not available!");
#endif

  if (!m_finished) {
    // Get pthread id
    pthread_t tid = m_worker.native_handle();

    if (tid) {
      // We cancel thread or ignore error if we cannot find it cause it already
      // finished
      int pthreadErrno;
      if ((pthreadErrno = pthread_cancel(tid)) != 0 && pthreadErrno != ESRCH)
        throw runtime_error {
          string { "Thread cancelation failed. Error: " }
          +::strerror(pthreadErrno)
        }
      ;

      // We join it
      m_worker.join();
    }

    // After cancelation thread is finished
    m_finished = true;
  }
}

bool CancelableThread::IsJoinable() const { return m_worker.joinable(); }

void CancelableThread::Join() {
  if (!IsJoinable())
    throw runtime_error { "Thread not joinable" }
  ;
  m_worker.join();
}

void CancelableThread::AllowCancel() {
  // Enable thread cancelation
  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr) != 0)
    throw runtime_error {
      string { "Enabling thread cancelation failed. Error: " }
      +::strerror(errno)
    }
  ;
  // Set thread to be cancelable at any time
  if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr) != 0)
    throw runtime_error {
      string { "Setting thread to be cancelable at any time failed. Error: " }
      +::strerror(errno)
    }
  ;

}
