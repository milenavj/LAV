#include "lav/Threads/ThreadPool.h"

// STL headers
#include <utility>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

using namespace Utils;
using namespace ThreadSafe;
using namespace std;

ThreadPool::ThreadPool(FixedQueue<function<int()>> &&tasks, int num)
    :m_tasks{move(tasks)}
{
    // Function for every thread
    auto f = [=](shared_ptr<FixedQueue<function<int()>>> queuePtr, const Event::Pointer &eptr)
    {
      //  cout << "Starting new thread" <<endl;
        // Get tasks and execute it
        while(!queuePtr->Empty())
        {
                auto taskPtr = queuePtr->Pop();
                int retval = (*taskPtr)();
                if(retval == 1400)
                {
                    cout << "Thread signal unsat" << endl;
                    eptr->Signal(Outcome::Unsat);
                    ::pause();
                 }
                //cout << "Thread signal finished" << endl;
        }

       // eptr->Signal(Outcome::Finished);
    };

    // Bind tasks
    auto queuePtr = make_shared<FixedQueue<function<int()>>>(move(m_tasks));
    for(auto i = 0; i<num; i++)
        m_threads.emplace_back(bind(f, queuePtr, placeholders::_1));

    // Create cancel event
    m_cancel_event = Event::Create();

    // Create control thread and force it to wait for events of working threads, and wait for cancel event
    m_control_thread = thread([this]()
    {

        vector<Event::Pointer> events;
        for(const auto &t : m_threads)
            events.push_back(t.ShareEvent());
        events.push_back(m_cancel_event);

        vector<size_t> vidxs = Event::WaitForEvents(events);

        // If some event has been signaled
        for(auto idx : vidxs)
        {
            Event::Sigval value = events[idx]->Value();
            cout << value << endl;
            if(value == Outcome::Unsat)
             {
                cout << "UNSAT shutting down threads..." << endl;
                for(auto &t : m_threads)
                       t.Cancel();
                m_active = false;
                break;
            }
            else if(value == Outcome::Finished)
            {
                cout << "SAT queue is empty." << endl;

                m_active = false;
                break;
            }
            else if(value == Outcome::Canceled)
            {
                cout << "Cancel from main." << endl;
                for(auto &t : m_threads)
                    t.Cancel();

                m_active = false;
                break;
            }
        }
   });

}


ThreadPool::ThreadPool(ThreadPool &&t)
    :m_tasks{move(t.m_tasks)},m_threads{move(t.m_threads)},m_active{t.m_active}
{}

ThreadPool& ThreadPool::operator=(ThreadPool&& oth)
{
    if(this != &oth)
    {
        m_tasks = move(oth.m_tasks);
        m_threads = move(oth.m_threads);
        m_active = oth.m_active;
    }
    return *this;
}

ThreadPool::~ThreadPool()
{
    this->ShutDown();
}

bool ThreadPool::IsActive()
{
    return m_active;
}

// Cancelation from caller
void ThreadPool::Cancel()
{
   m_cancel_event->Signal(Outcome::Canceled);

}

// Join control thread to shutdown thread pool
void ThreadPool::ShutDown()
{
    if(m_control_thread.joinable())
        m_control_thread.join();
}
