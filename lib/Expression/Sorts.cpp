#include "expression/expressions/Sorts.h"

namespace argo {
thread_local SortRegistry *SortRegistry::_instance = 0;

pthread_mutex_t varsr= PTHREAD_MUTEX_INITIALIZER;

SortRegistry *SortRegistry::Instance() {
    pthread_mutex_lock(&varsr);
    if (!_instance)
      _instance = new SortRegistry();
    pthread_mutex_unlock(&varsr);
    return _instance;
  }

}
