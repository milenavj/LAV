#ifndef __NUMERAL_TYPES_H__
#define __NUMERAL_TYPES_H__

#include <gmpxx.h>
namespace argo {

enum IntType {
  noInt = 0,
  uInt1Type = 1,
  sInt1Type = 2,
  uInt8Type = 7,
  sInt8Type = 8,
  uInt16Type = 15,
  sInt16Type = 16,
  uInt32Type = 31,
  sInt32Type = 32,
  uInt64Type = 63,
  sInt64Type = 64
};

typedef mpz_class INT;
typedef mpq_class RATIONAL;
//typedef long long INT;
//typedef long double RATIONAL;
}
#endif
