#ifndef __HASH_SET_H__
#define __HASH_SET_H__

#if defined(__GNUCPP__)
#include <ext/hash_set>
using __gnu_cxx::hash_set;
using __gnu_cxx::hash;

#elif defined(__MSVC__)
#include <hash_set>
#include "hash_fun.h"
using stdext::hash;
using stdext::hash_set;
using stdext::hash_compare;
#endif

#endif
