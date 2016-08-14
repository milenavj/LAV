#include "config.h"
#ifdef Z3

#include "solvers/solver-interfaces/z3/bv-expression-z3.hpp"

namespace UrsaMajor {
std::map<std::string, Z3_func_decl> BVExpressionImpZ3::_uf_registry;
} // namespace UrsaMajor

#endif
