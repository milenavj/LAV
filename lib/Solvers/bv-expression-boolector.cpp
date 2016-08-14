#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)

#include "config.h"

#include "solvers/solver-interfaces/boolector/bv-expression-boolector.hpp"

namespace UrsaMajor {

#ifdef BOOLECTOR
std::map<std::string, SOLVER_EXPR_TYPE> BVExpressionImpBoolector::_uf_registry;
#endif

} // namespace UrsaMajor

#endif
