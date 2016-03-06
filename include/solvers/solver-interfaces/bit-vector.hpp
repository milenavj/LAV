#ifndef BITVECTOR_HPP
#define BITVECTOR_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <assert.h>

#include "functional-ext.hpp"

namespace UrsaMajor {

//FIXME mozda izbaciti ovu klasu?
class BitVector {
public:
  typedef unsigned char byte;
  
  explicit BitVector(size_t nBits) 
    : _nBits(nBits) {
    _bytes.resize(numBytesRequired());
    std::fill(_bytes.begin(), _bytes.end(), 0);
  }

  static BitVector fromInteger(unsigned int n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(unsigned);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(true);
    return b;
  }

  static BitVector fromInteger(signed int n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(int);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(false);
    return b;
  }

  static BitVector fromInteger(unsigned long n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(unsigned long);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(true);
    return b;
  }

  static BitVector fromInteger(signed long n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(signed long);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(false);
    return b;
  }
  
  static BitVector fromInteger(unsigned short n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(unsigned short);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(true);
    return b;
  }

  static BitVector fromInteger(signed short n, size_t nBits = 0) {
    if (nBits == 0) nBits = 8*sizeof(short);
    BitVector b(nBits);
    for (size_t i = 0; i < b.numBytesRequired(); i++) {
      b._bytes[i] = n;
      n >>= 8;
    }
    b.resetExcessBits();
//    b.SetUnsigned(false);
    return b;
  }

  bool isZero() const
  {
  BitVector b(_nBits);
  if((*this) == b) return true;
  return false;
  }

  int SignBit() const
  {
    if(getBit(_bytes[_bytes.size() - 1], 7) == false) return 0;
    else return 1; 
  }

/*  bool isUnsigned() const
  {
  return _bUnsigned;
  }

  bool isSigned() const
  {
  return !_bUnsigned;
  }
*/
/*  void SetUnsigned(bool b)
  {
  _bUnsigned = b;
  }
*/
  unsigned long toUnsigned() const {
//FIXME long long
    if (_nBits > 8*sizeof(unsigned long long))
      throw "Too large to fit into unsigned";
    unsigned long result = 0;
    for (int i = _bytes.size() - 1; i >= 0; i--) {
      result <<= 8;
      result |= _bytes[i];
    }
    return result;
  }

  void print(std::ostream& ostr) const {
/*
    for (size_t i = 0; i < _bytes.size(); i++) {
      for (int j = 7; j >= 0; j--)
	ostr << getBit(_bytes[i], j);
      ostr << ".";
    }
    ostr << std::endl;
*/

    for (int i = _nBits - 1; i >= 0; i--)
      ostr << (getBit(i) ? "1" : "0");
 //   ostr << std::endl;
  }

  std::string toString() const {
    std::ostringstream ostr;
    print(ostr);
    return ostr.str();
  }

  bool getBit(size_t n) const {
    if (n >= _nBits)
      throw "NonExistent bit in word";

    return getBit(_bytes[n / 8], n % 8);
  }

  void setBit(size_t n) {
    if (n >= _nBits)
      throw "NonExistent bit in word";
    _bytes[n / 8] = setBit(_bytes[n / 8], n % 8);
  }

  void resetBit(size_t n) {
    if (n >= _nBits)
      throw "NonExistent bit in word";
    _bytes[n / 8] = resetBit(_bytes[n / 8], n % 8);
  }

  void toggleBit(size_t n) {
    if (n >= _nBits)
      throw "NonExistent bit in word";
    _bytes[n / 8] = toggleBit(_bytes[n / 8], n % 8);
  }

  BitVector operator& (const BitVector& b) const {
    return applyOperator(b, bit_and<byte>());
  }

  BitVector operator| (const BitVector& b) const {
    return applyOperator(b, bit_or<byte>());
  }

  BitVector operator^ (const BitVector& b) const {
    return applyOperator(b, bit_xor<byte>());
  }

  template <class T>
  struct bit_not : public std::unary_function<T,T>
  {
    T operator()(const T& x) const { return ~x; }
  };

  BitVector operator~() const {
    return applyOperator(bit_not<byte>());
  }

  BitVector operator+(const BitVector& b) const {
    if (_nBits != b._nBits)
      {
      print(std::cout);
      b.print(std::cout);
      throw "Different length operator+";
      }

    BitVector result(_nBits);
    unsigned carry = 0;
    for (size_t i = 0; i < _bytes.size(); i++) {
      unsigned sum = this->_bytes[i] + b._bytes[i] + carry;
      carry = sum >> 8;
      result._bytes[i] = (byte)(sum & 0xFF);
    }
    result.resetExcessBits();
    return result;
  }

  BitVector operator-() const {
    BitVector result = ~(*this);
    for (size_t i = 0; i < result._bytes.size(); i++)
      if (++result._bytes[i] != 0)
	break;
    result.resetExcessBits();
    return result;
  }

  BitVector operator-(const BitVector& b) const {
    return (*this) + (-b);
  }

  BitVector operator*(const BitVector& b) const {
    if (_nBits != b._nBits)
{
      print(std::cout);
      b.print(std::cout);
      throw "Different length operator*";
 } 
    BitVector result(_nBits);
    for(size_t i = 0; i < this->_bytes.size(); i++) {
      unsigned carry = 0;
      for(size_t j = 0; i + j < b._bytes.size(); j++) {
        unsigned val = (this->_bytes[i] * b._bytes[j]) +
	  result._bytes[i+j] + carry;

        result._bytes[i+j] = (byte)(val & 0xFF);
        carry = val >> 8;
      }
    }
    result.resetExcessBits();
    return result;
  }

//FIXME napraviti pravo deljenje i ostatak pri deljenju

  BitVector sdiv(const BitVector& b) const {
    if (_nBits != b._nBits)
      throw "Different length sdiv";
  
    BitVector result(_nBits);
    if(b.isZero()) throw "Division by zero";
    if(isZero()) return result;

    signed long n1 = toUnsigned();
    signed long n2 = b.toUnsigned();
    return fromInteger(n1/n2, _nBits);
  }

  BitVector udiv(const BitVector& b) const {
    if (_nBits != b._nBits)
      throw "Different length udiv";
  
    BitVector result(_nBits);
    if(b.isZero()) throw "Division by zero";
    if(isZero()) return result;

    unsigned long n1 = toUnsigned();
    unsigned long n2 = b.toUnsigned();
    return fromInteger(n1/n2, _nBits);
  }

  BitVector urem(const BitVector& b) const {
    if (_nBits != b._nBits)
      throw "Different length urem";
  
    BitVector result(_nBits);

    if(b.isZero()) throw "% by zero";
    if(this->isZero()) return result;
 
      unsigned long n1 = toUnsigned();
      unsigned long n2 = b.toUnsigned();
      return fromInteger(n1%n2, _nBits);
  }

  BitVector srem(const BitVector& b) const {
    if (_nBits != b._nBits)
      throw "Different length srem";
  
    BitVector result(_nBits);

    if(b.isZero()) throw "% by zero";
    if(this->isZero()) return result;
 
      signed long n1 = toUnsigned();
      signed long n2 = b.toUnsigned();
      return fromInteger(n1%n2, _nBits);
  }


  BitVector operator<<(unsigned n) const {
    return shiftLeft(n);
  }

  bool sgt(const BitVector& b) const  {
      return this->s_compare(b) < 0;
  }

  bool sge(const BitVector& b) const  {
      return this->s_compare(b) <= 0;
  }

  bool slt(const BitVector& b) const  {
      return this->s_compare(b) > 0;
  }

  bool sle(const BitVector& b) const  {
      return this->s_compare(b) >= 0;
  }


  bool ugt(const BitVector& b) const  {
      return this->u_compare(b) < 0;
  }

  bool uge(const BitVector& b) const  {
      return this->u_compare(b) <= 0;
  }

  bool ult(const BitVector& b) const  {
      return this->u_compare(b) > 0;
  }

  bool ule(const BitVector& b) const  {
      return this->u_compare(b) >= 0;
  }

  bool operator==(const BitVector& b) const {
    return this->s_compare(b) == 0;
  }

  bool operator!=(const BitVector& b) const {
    return this->s_compare(b) != 0;
  }

  size_t size() {
    return _nBits;
  }

//    for (size_t i = 0; i < b.numBytesRequired(); i++) {
//      b._bytes[i] = n;

  BitVector zext(size_t width) const {
    BitVector result(width);
    for(size_t i=0; i<_bytes.size(); i++)
        result._bytes[i] = _bytes[i];
    return result;
  }

  //FIXME nisam sigurna kako ovo radi za unsigned - da li i tada treba dodati 1
  BitVector sext(size_t width) const {
    BitVector result(width);
    for(size_t i=0; i<_bytes.size(); i++)
        result._bytes[i] = _bytes[i];
//    if(isNegative())
      if(SignBit())
        for(unsigned i=_bytes.size(); i < result.numBytesRequired(); i++)
          result._bytes[i]  = -1;
    return result;
  }

//FIXME ovo je sumnjivo
  BitVector extract(size_t low, size_t high) const {
    BitVector result(high-low+1);
    for(size_t i=0; i<(high-low+1)/8; i++)
        result._bytes[i] = _bytes[i];
    return result;
  }


  BitVector aShiftRight(unsigned n) const {
    BitVector result = *this;
    size_t shiftAmount = 8;
    while (n > 0) {
      if (n < shiftAmount)
        shiftAmount = n;
      n -= shiftAmount;
      result.aShiftRightOnce(shiftAmount);
    }
    return result;
  }
  
  BitVector lShiftRight(unsigned n) const {
    BitVector result = *this;
    size_t shiftAmount = 8;
    while (n > 0) {
      if (n < shiftAmount)
        shiftAmount = n;
      n -= shiftAmount;
      result.lShiftRightOnce(shiftAmount);
    }
    return result;
  }

size_t getNBits() const
{return _nBits;}

private:

int u_compare(const BitVector& b) const {
    int i;
//    unsigned size = _bytes.size() - 1;
    for (i = this->_bytes.size() - 1; this->_bytes[i] == b._bytes[i]; i--)
      if (i == 0)
        return 0;
     return (signed)b._bytes[i] - (signed)_bytes[i];
  }


  int s_compare(const BitVector& b) const {
    int i;

    for (i = this->_bytes.size() - 1; this->_bytes[i] == b._bytes[i]; i--)
      if (i == 0)
        return 0;
        
    //ovaj je negativan b je pozitivan
    if(SignBit() > b.SignBit()) return 1;

    //ovaj je pozitivan b je negativan
    if(SignBit() < b.SignBit()) return -1;

    return  (signed)b._bytes[i] - (signed)_bytes[i];

  }


  template <typename _BinaryOperation>
  BitVector applyOperator(_BinaryOperation op) const {
    BitVector result(_nBits);
    for (size_t i = 0; i < _bytes.size(); i++)
      result._bytes[i] = op(_bytes[i]);
    result.resetExcessBits();
    return result;
  }

  template <typename _BinaryOperation>
  BitVector applyOperator(const BitVector& b, _BinaryOperation op) const {
    if (_nBits != b._nBits)
      throw "Different length applyOperator";
    BitVector result(_nBits);
    for (size_t i = 0; i < _bytes.size(); i++)
      result._bytes[i] = op(this->_bytes[i], b._bytes[i]);
    return result;
  }

  BitVector shiftLeft(unsigned n) const {
    BitVector result = *this;
    size_t shiftAmount = 8;
    while (n > 0) {
      if (n < shiftAmount)
	shiftAmount = n;
      n -= shiftAmount;
      result.shiftLeftOnce(shiftAmount);
    }
    return result;
  }
  
  void shiftLeftOnce(size_t n) {
//    if (!(0 <= n && n <= 8)) {
    if (!(n <= 8)) {
      throw "shiftLeftOnce:: n must be between 0 and 8";
    }
    unsigned carry = 0;
    for(size_t i = 0; i < this->_bytes.size(); i++) {
      unsigned val = (unsigned)_bytes[i] << n;
      val |= carry;
      _bytes[i] = (byte)(val & 0xFF);
      carry = (val & 0xFF00) >> 8;
    }
  }

  void lShiftRightOnce(size_t n) {
//    if (!(0 <= n && n <= 8)) {
    if (!(n <= 8)) {
      throw "shiftRightOnce:: n must be between 0 and 8";
    }
    signed carry = 0;
    for(int i = this->_bytes.size() - 1; i >=0 ; i--) {
      unsigned val = (unsigned)_bytes[i] >> n;
      val |= carry;
      carry = (unsigned)_bytes[i] << (8 - n);
      _bytes[i] = (byte)val;
    }
  }

  void aShiftRightOnce(size_t n) {
//    if (!(0 <= n && n <= 8)) {
    if (!(n <= 8)) {
      throw "shiftRightOnce:: n must be between 0 and 8";
    }
    signed carry;
//    if(isPositiveOrZero()) carry = 0;
//      else carry = -1;
    if(SignBit()) carry = -1;
    else carry = 0;
    for(int i = this->_bytes.size() - 1; i >=0 ; i--) {
      unsigned val = (unsigned)_bytes[i] >> n;
      val |= carry;
      carry = (unsigned)_bytes[i] << (8 - n);
      _bytes[i] = (byte)val;
    }
  }

  byte setBit(byte b, size_t n) {
    if (n > 7)
      throw "NonExistent bit";
    return b | (1 << n);
  }

  byte resetBit(byte b, size_t n) {
    if (n > 7)
      throw "NonExistent bit";
    return b & ~(1 << n);
  }

  bool getBit(byte b, size_t n) const {
    if (n > 7)
      throw "NonExistent bit";
    return b & (1 << n);
  }

  byte toggleBit(byte b, size_t n) const {
    if (n > 7)
      throw "NonExistent bit";
    return b ^ (1 << n);
  }

  size_t numBytesRequired() const {
    return (_nBits - 1) / 8 + 1;
  }

  void resetExcessBits() {
    int excessBits = 8*_bytes.size() - _nBits;
    _bytes.back() &= 0xFFu >> excessBits;
  }


  bool _bUnsigned;
  size_t _nBits;
  std::vector<byte> _bytes;
};

}//end of namespace

#endif
