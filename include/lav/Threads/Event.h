#ifndef EVENT_HPP
#define EVENT_HPP

// STL headers
#include <memory>
#include <cstddef>
#include <vector>
#include <chrono>

namespace Utils
{
    class Event
    {
    public:
        using Pointer = std::shared_ptr<Event>;
        using Sigval = uint64_t;

        static constexpr int InvalidEvent = -1;
    private:
        int m_fd;
        bool m_ready;
        // Event is movable
        Event();
        Event(Event &&e);
        Event& operator=(Event &&e);
        ~Event();

        // User cannot delete pointer
        static void Deleter(Event *shared);
    public:
        // Creates shared pointer which will be shared between threads
        static Pointer Create();

        // Ready() check if we can get value from Event object
        // Signal() signal a notification to waiting thread
        // Value() gets notification value
        // WaitForEvents() waint until one of the events return
        bool Ready() const;
        void Signal(Sigval value = 1) const;
        Sigval Value(bool block = false);
        static std::vector<std::size_t> WaitForEvents(std::vector<Event::Pointer> &events,
                                                      const std::chrono::milliseconds &waitMs = {},
                                                      bool block = true);
    };
}
#endif // EVENT_HPP
