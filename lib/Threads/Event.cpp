// Project headers
#include "lav/Threads/Event.h"

// STL headers
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

// Other headers
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace Utils;
using namespace std;

// Creates new event file descriptor and initialize value to 0
Event::Event() : m_fd { ::eventfd(0, EFD_SEMAPHORE) }
, m_ready { false }
, m_unsat { false }
, m_cancel { false }
{
    if (m_fd == InvalidEvent)
        throw runtime_error(string {
        "Couldn't create new Event object. Error: "
    } + ::strerror(errno));
}

Event::Event(Event &&e) : m_fd { e.m_fd }
, m_ready { e.m_ready }
, m_unsat { e.m_unsat }
, m_cancel { e.m_cancel }
{
    e.m_fd = InvalidEvent;
    e.m_ready = false;
}

Event &Event::operator=(Event &&e) {
    if (this != &e) {
        m_fd = e.m_fd;
        m_ready = e.m_ready;
        e.m_fd = InvalidEvent;
        e.m_ready = false;
    }
    return *this;
}

Event::~Event() {
    if (m_fd != InvalidEvent)
        ::close(m_fd);
}

void Event::Deleter(Event *shared) { delete shared; }

Event::Pointer Event::Create() {
    return Pointer { new Event
    {}
    , &Event::Deleter
}
;
}

bool Event::Ready() const { return m_ready; }

void Event::Signal(Sigval value) {
    uint64_t buff = 1;

    if (value == Outcome::Canceled)
      m_cancel = true;
    else if (value == Outcome::Unsat)
      m_unsat = true;

    // Sending signal, writing to file descriptor
    if (::write(m_fd, &buff, sizeof(uint64_t)) == -1)
        throw runtime_error {
          string { "Sending signal via Event failed. Error: " }
          +::strerror(errno)
        }
    ;
}

Event::Sigval Event::Value(bool block) {
    // Check if we can get value
    if (!m_ready && !block)
        throw runtime_error {
          string { "Event not ready" }
        }
    ;

    // Get value
    uint64_t buff;
    if (::read(m_fd, &buff, sizeof(uint64_t)) == -1)
        throw runtime_error {
          string { "Reading Event value failed. Error: " }
          +::strerror(errno)
        }
    ;

    // Undo ready and return
    if (m_cancel) {
        m_ready = false;
        return Outcome::Canceled;
    } else if (m_unsat) {
        m_ready = false;
        return Outcome::Unsat;
    } else
        return Outcome::Sat;
}

vector<size_t> Event::WaitForEvents(std::vector<Event::Pointer> &events,
                                    const std::chrono::milliseconds &waitMs,
                                    bool block) {
    using poll_t = struct pollfd;
    vector<poll_t> pevents;
    pevents.reserve(events.size());

    // Make vector of events for poll syscall
    transform(events.cbegin(), events.cend(), back_inserter(pevents),
              [ = ](const Event::Pointer & eptr) {
        poll_t p;
        p.events = POLLIN;
        p.fd = eptr->m_fd;
        p.revents = 0;
        return p;
  });

    // Poll
    vector<size_t> readyEvents;
    int iWaitMs = static_cast<int>(waitMs.count());
    // -1 if we want to block until event occurs, or 0 if we want to return
    // immediately
    iWaitMs = !iWaitMs && block ? -1 : iWaitMs;
    int eventsOccured = ::poll(pevents.data(), pevents.size(), iWaitMs);

    // CHeck for error
    if (eventsOccured == -1)
        throw runtime_error {
          string { "Waiting for events failed. Error: " }
          +::strerror(errno)
        }
    ;

    // Make vector of indexes of ready events
    if (eventsOccured) {
        readyEvents.reserve(eventsOccured);

        for (auto it = pevents.cbegin(); it != pevents.cend(); ++it) {
            // Get index of event
            size_t idx = it - pevents.cbegin();

            if (it->revents == POLLIN) {
                readyEvents.push_back(idx);
                events[idx]->m_ready = true;
            } else if (it->revents == POLLERR)
                throw runtime_error {
                  string { "Error when waiting for event: " }
                  +to_string(idx) + " Error: " + ::strerror(errno)
                }
            ;

        }
    }

    return readyEvents;
}
