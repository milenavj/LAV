#ifndef EVENT_HPP
#define EVENT_HPP

// STL zaglavlja
#include <memory>
#include <cstddef>
#include <vector>
#include <chrono>

namespace Threads {
class Event {
public:
  enum Outcome {
    Unsat = 1,
    Finished = 2
  };
  //using Pointer = std::shared_ptr<Event>;
  using Sigval = uint64_t;

  thread_local static constexpr int InvalidEvent = -1;

private:
  int m_fd;
  bool m_ready;
  bool m_unsat;
  bool m_finished;

  Event();
  Event(Event &&e);
  Event &operator=(Event &&e);
  ~Event();

  static void Deleter(Event *shared);

public:

  // Kreiramo pokazivac koji ce niti da dele
  static std::shared_ptr<Event> Create();

  // Ispitujemo da li se desio neki dogadjaj
  bool Ready() const;
  // Signalizira
  void Signal(Sigval value = 1);
  // Citamo vrednost koja je signalizirana
  Sigval Value(bool block = false);
  // Cekamo dok se ne desi neki signal
  thread_local static std::vector<std::size_t>
      WaitForEvents(std::vector<std::shared_ptr<Event> > &events,
                    std::chrono::milliseconds waitMs = {
  });

};
}

#endif // EVENT_HPP
