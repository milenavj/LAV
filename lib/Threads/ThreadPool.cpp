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

ThreadPool::ThreadPool(FixedQueue<std::function<int()>> &&tasks, int num)
    :m_tasks{std::move(tasks)},m_num_threads{num}
{
  m_num_tasks = m_tasks.Size();

  InitWorkerThreads();
  Start();
}


ThreadPool::ThreadPool(ThreadPool &&t)
    :m_tasks{std::move(t.m_tasks)},m_threads{std::move(t.m_threads)},m_active{t.m_active}
{}

ThreadPool& ThreadPool::operator=(ThreadPool&& oth)
{
    if(this != &oth)
    {
        m_tasks = std::move(oth.m_tasks);
        m_threads = std::move(oth.m_threads);
        m_active = oth.m_active;
    }
    return *this;
}

ThreadPool::~ThreadPool()
{
    this->JoinAll();
    this->ShutDown();
}

bool ThreadPool::IsActive()
{
    return m_active;
}

// Making working lambda for every worker
void ThreadPool::InitWorkerThreads()
{
    // Function for every thread
    auto f = [=](std::shared_ptr<FixedQueue<std::function<int()>>> queuePtr, const Event::Pointer &eptr)
    {
      //  cout << "Starting new thread" <<endl;
        // Get tasks and execute it
        while(!queuePtr->Empty())
        {
                auto taskPtr = queuePtr->Pop();
                if(taskPtr != nullptr)
                {
                    int retval = (*taskPtr)();
                    if(retval == -1)
                    {
                        std::cout << "Thread signal unsat" << std::endl;
                        eptr->Signal(Utils::Event::Outcome::Unsat);
                     }
                    else
                    {
                        std::cout << "Thread signal sat" << std::endl;
                        eptr->Signal(Utils::Event::Outcome::Sat);
                        //this_thread::sleep_for(chrono::seconds{1});
                    }
                //cout << "Thread signal finished" << endl;
              }
              else
                break;
        }

     //  eptr->Signal(Outcome::Finished);
    };

    //cout << "NUM THREADS = " << m_num_threads << endl;
    // Bind tasks
    auto queuePtr = std::make_shared<FixedQueue<std::function<int()>>>(std::move(m_tasks));
    for(auto i = 0; i<m_num_threads; i++)
        m_threads.emplace_back(std::bind(f, queuePtr, std::placeholders::_1));

}

// Making control thread and start working threads
void ThreadPool::Start()
{

    // Create cancel event
    m_cancel_event = Event::Create();

    // Create control thread and force it to wait for events of working threads, and wait for cancel event
    m_control_thread = std::thread([this]()
    {

        std::vector<Event::Pointer> events;
        for(const auto &t : m_threads)
            events.push_back(t.ShareEvent());
        events.push_back(m_cancel_event);

        bool cancel = false;
        int num_finished_tasks = 0;
        while(num_finished_tasks != m_num_tasks && !cancel)
        {
            std::vector<size_t> vidxs = Event::WaitForEvents(events);
            // If some event has been signaled
            for(auto idx : vidxs)
            {
                num_finished_tasks += 1;

                Event::Sigval value = events[idx]->Value();

                // cout << "NUM FINISHED " << num_finished_tasks << "  signal " <<  value << endl;
               // Event::Sigval value = events[idx]->Value();
                if(value == Utils::Event::Outcome::Unsat)
                 {
                    std::cout << "UNSAT shutting down threads..." << std::endl;
                    for(auto &t : m_threads)
                           t.Cancel();
                    m_active = false;
                    cancel = true;
                    break;
                }
                else if(value == Utils::Event::Outcome::Finished)
                {
                    std::cout << "SAT." << std::endl;

                    m_active = false;
                }
                else if(value == Utils::Event::Outcome::Canceled)
                {
                    std::cout << "Cancel from main." << std::endl;
                    for(auto &t : m_threads)
                        t.Cancel();

                    cancel = true;
                    m_active = false;
                    break;
                }
                if(cancel)
                   break;
            }
        }
        if(num_finished_tasks == m_num_tasks)
            std::cout << "All threads signal SAT" << std::endl;
   });
  if(m_control_thread.joinable())
        m_control_thread.join();

}

// Cancelation from caller
void ThreadPool::Cancel()
{
   m_cancel_event->Signal(Utils::Event::Outcome::Canceled);

}

// Join all threads
void ThreadPool::JoinAll()
{
  for(auto &t : m_threads)
      t.Join();
}

// Join control thread to shutdown thread pool
void ThreadPool::ShutDown()
{
  if(m_control_thread.joinable())
        m_control_thread.join();
}
