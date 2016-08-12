#include "config.h"

#ifdef BOOLECTOR

#include "solvers/solver-interfaces/boolector/bv-expression-boolector.hpp"

namespace UrsaMajor {
std::map<std::string, SOLVER_EXPR_TYPE> BVExpressionImpBoolector::_uf_registry;
} // namespace UrsaMajor

#endif
