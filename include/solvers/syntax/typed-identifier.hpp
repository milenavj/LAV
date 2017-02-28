#ifndef __TYPED_IDENTIFIER_HPP__
#define __TYPED_IDENTIFIER_HPP__

#include "type.hpp"

namespace UrsaMajor {
typedef std::pair<Type, std::string> TypedId;
#define id_type(x) (x).first
#define id_name(x) (x).second
}

#endif
