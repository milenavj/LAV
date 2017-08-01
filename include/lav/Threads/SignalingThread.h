#ifndef SIGNALINGTHREAD_H
#define SIGNALINGTHREAD_H

#include "CancelableThread.h"
#include "Event.h"

// STL zaglavlja
#include <functional>

namespace Threads {
struct SignalingThreadBaseFromMember {
  std::shared_ptr<Event::Event> m_event;
  explicit SignalingThreadBaseFromMember();
  SignalingThreadBaseFromMember(SignalingThreadBaseFromMember &&st);

  SignalingThreadBaseFromMember &operator=(SignalingThreadBaseFromMember &&st);
  ~SignalingThreadBaseFromMember();
};

class SignalingThread : protected SignalingThreadBaseFromMember,
                        public CancelableThread {
public:
  using Callable = std::function<void(const std::shared_ptr<Event::Event> &)>;
  SignalingThread(Callable f);
  SignalingThread(SignalingThread &&st);
  SignalingThread &operator=(SignalingThread &&st);
  virtual ~SignalingThread();

  const std::shared_ptr<Event::Event> &ShareEvent() const;
};
}

#endif // SIGNALINGTHREAD_H
