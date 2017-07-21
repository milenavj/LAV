#include "lav/Threads/ThreadPool.h"

// STL zaglavlja
#include <utility>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

using namespace Threads;

#include "llvm/Support/CommandLine.h"
extern llvm::cl::opt<bool> FindFirstFlawed;

ThreadPool::ThreadPool() {}

ThreadPool::ThreadPool(std::vector<std::function<int()> > &&tasks,
                       uint64_t num_threads)
    : m_tasksPtr {
  std::make_shared<FixedQueue<std::function<int()> > >(
      FixedQueue<std::function<int()> >(std::move(tasks)))
}
, m_num_threads { num_threads }
, m_num_tasks { tasks.size() }
{}

ThreadPool::ThreadPool(ThreadPool &&tp) : m_tasksPtr {
  std::move(tp.m_tasksPtr)
}
, m_threads { std::move(tp.m_threads) }
, m_num_tasks { tp.m_num_tasks }
{}

ThreadPool &ThreadPool::operator=(ThreadPool &&oth) {
  if (&oth != this) {
    m_tasksPtr = std::move(oth.m_tasksPtr);
    m_threads = std::move(oth.m_threads);
    m_num_threads = oth.m_num_threads;
    m_num_tasks = oth.m_num_tasks;
  }
  return *this;
}

ThreadPool::~ThreadPool() {
  JoinControlThread();
  JoinWorkerThreads();
}

void ThreadPool::JoinControlThread() {
  if (m_control_thread.joinable())
    m_control_thread.join();
}

void ThreadPool::JoinWorkerThreads() {
  for (auto i = 0U; i < m_num_threads; i++)
    if (m_threads[i].GetHandle().joinable())
      m_threads[i].GetHandle().join();
}

void ThreadPool::DetachWorkerThreads() {
  for (auto i = 0U; i < m_num_threads; i++)
    m_threads[i].GetHandle().detach();
}

void ThreadPool::Init(std::vector<std::function<int()> > &&tasks,
                      uint64_t num_threads) {
  m_tasksPtr = std::make_shared<FixedQueue<std::function<int()> > >(
      FixedQueue<std::function<int()> >(std::move(tasks)));
  m_num_threads = num_threads;
  m_num_tasks = tasks.size();
}

void ThreadPool::StartWorkerThreads() {
  // Funkcija koju izvrsava svaka nit
  auto f = [this](const Event::Pointer & eptr) {
    // Uzima zadatak po zadatak i izvrsava ga
    while (!m_tasksPtr->Empty()) {
      auto taskPtr = m_tasksPtr->Pop();
      if (taskPtr == nullptr)
        break;
      int retval = (*taskPtr)();

      // Ako smo dosli do UNSAT-a, signaliziramo
      if (retval == -1) {
        eptr->Signal(Event::Outcome::Unsat);
        break;
      }
    }
    // Ako nema vise zadataka u redu, signaliziramo
    eptr->Signal(Event::Outcome::Finished);

  }
  ;

  for (auto i = 0U; i < m_num_threads; i++)
    m_threads.emplace_back(f);
}

void ThreadPool::StartControlThread() {
    // Pravimo kontrolnu nit koja ceka na signale
  m_control_thread = std::thread([this]() {

    std::vector<Event::Pointer> events;
    for (const auto &t : m_threads)
      events.push_back(t.ShareEvent());

    uint64_t num_finished_tasks = 0;
    int cancel = 0;
    while (num_finished_tasks < m_num_threads) {

      std::vector<size_t> vidxs = Event::WaitForEvents(events);
      // Ukoliko se neki event dogodio
      for (auto idx : vidxs) {
        num_finished_tasks += 1;

        Event::Sigval value = events[idx]->Value();

        if (value == Event::Outcome::Unsat || cancel) {
          for (auto &t : m_threads)
            t.Cancel();
          std::cout << "Task finished unsuccessfully." << std::endl;
          cancel = 1;
          m_result = ThreadPool::Unsucc;
          break;
        }

        std::cout << "Task finished successfully." << std::endl;
      }
      if (cancel)
        break;
    }

    if (!cancel) {
      m_result = ThreadPool::Succ;
      std::cout << "All tasks are finished successfully." << std::endl;
    }
  });
}

void ThreadPool::Work() {
  StartWorkerThreads();
  if (FindFirstFlawed) {
    //        DetachWorkerThreads();
    StartControlThread();
    JoinControlThread();
    //        JoinWorkerThreads();
  } else
    JoinWorkerThreads();
}

ThreadPool::Result ThreadPool::GetResult() { return m_result; }
