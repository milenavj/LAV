#ifndef FIXEDQUEUE_HPP
#define FIXEDQUEUE_HPP

// STL headers
#include <vector>
#include <atomic>
#include <memory>
#include <cstddef>
#include <utility>

namespace ThreadSafe {

//template <typename T>
//class FixedQueue2
//{
//public:
//    std::vector<std::function<T ()>> pera;
//};
template <typename T> class FixedQueue {
public:
  using ValueType = T;

private:
  std::vector<T> m_data;
  std::unique_ptr<std::atomic_uint> m_idxPtr;

public:
  FixedQueue(const std::vector<T> &data) : m_data { data }
  , m_idxPtr {
    new std::atomic_uint { 0 }
  }
  {}

  FixedQueue(FixedQueue &&fq) : m_data { std::move(fq.m_data) }
  , m_idxPtr { std::move(fq.m_idxPtr) }
  {}

  FixedQueue &operator=(FixedQueue &&fq) {
    if (&fq != this) {
      m_data = std::move(fq.m_data);
      m_idxPtr = std::move(fq.m_idxPtr);
    }
    return *this;
  }

  ~FixedQueue() {}

  inline std::size_t Size() const { return m_data.size(); }

  inline bool Empty() const {
    uint idx = m_idxPtr->load();
    return idx == m_data.size();
  }

  // Returns a pointer (nullptr in case of error)
  T *Pop() {
    // Get index
    uint idx = m_idxPtr->fetch_add(1U);

    // Check index
    if (idx > Size())
      return nullptr;

    return &m_data[idx];
  }
};
}

#endif
