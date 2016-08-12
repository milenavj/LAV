#ifndef __FUNCTIONAL_EXT_HPP_
#define __FUNCTIONAL_EXT_HPP_

#include <functional>
namespace UrsaMajor {

template <class T> struct bit_and;
template <class T> struct bit_or;
template <class T> struct bit_xor;

template <typename _Tp>
struct bit_and : public std::binary_function<_Tp, _Tp, _Tp> {
  _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x & __y; }
};

template <typename _Tp>
struct bit_or : public std::binary_function<_Tp, _Tp, _Tp> {
  _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x | __y; }
};

template <typename _Tp>
struct bit_xor : public std::binary_function<_Tp, _Tp, _Tp> {
  _Tp operator()(const _Tp &__x, const _Tp &__y) const { return __x ^ __y; }
};

template <class T> struct bit_not : public std::unary_function<T, T> {
  T operator()(const T &x) const { return ~x; }
};
} //end of namespace
#endif
