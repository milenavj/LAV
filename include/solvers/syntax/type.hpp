#ifndef __TYPE_HPP__
#define __TYPE_HPP__

enum _Type {
  UNSIGNED,
  BOOLEAN,
  BITVECTOR,
  ARRAY,
  STATEMENT,
  ERROR,
  SQL
};

namespace UrsaMajor {
class Type {
public:
  Type(enum _Type t, unsigned short width = 0, bool s = true)
      : _type(t), _width(width), _signed(s) {}

  enum _Type getType() const { return _type; }

  unsigned getWidth() const { return _width; }

  bool isSigned() const { return _signed; }

private:
  enum _Type _type;
  unsigned _width;
  bool _signed;
};
}

#endif
