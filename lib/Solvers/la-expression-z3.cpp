#include "config.h"
#ifdef Z3

#include "llvm/Support/CommandLine.h"

#include "solvers/solver-interfaces/z3/la-expression-z3.hpp"

extern llvm::cl::opt<bool> DumpSMT;

namespace UrsaMajor {

void LAExpressionImpZ3::print(Z3_ast exp) const {
    if(DumpSMT)
        std::cerr << Z3_ast_to_string (getSolver(), exp) << std::endl;
}

void LAExpressionImpZ3::print(Z3_func_decl f) const {
    if(DumpSMT)
        std::cerr << Z3_func_decl_to_string(getSolver(), f) << std::endl;
}
void LAExpressionImpZ3::print(Z3_sort sort) const {
    if(DumpSMT)
        std::cerr << Z3_sort_to_string(getSolver(), sort) << std::endl;
}

void LAExpressionImpZ3::print(Z3_ast expr, Z3_sort sort) const {
    if(!DumpSMT) return;
    std::cerr << "(declare-const "
              << std::string(Z3_ast_to_string(getSolver(), expr)) << " "
              << std::string(Z3_sort_to_string(getSolver(), sort))
              << ")"
              << std::endl;
}



} // namespace UrsaMajor

#endif
