#ifndef __Z3_INSTANCE__
#define __Z3_INSTANCE__

#include <string>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <stdlib.h>

#include "solvers/solver-interfaces/auxiliary.hpp"

extern "C" {
//#include "z3_v1.h"
#include "z3.h"
}

namespace UrsaMajor {


  class Z3Instance {
  public:
    static Z3Instance& instance();

    ~Z3Instance();

    Z3_context getSolver() {
      return _ctx;
    }

    bool nextModel(Z3_ast expr);
    bool addConstraint(Z3_ast expr) ;
    bool addTempConstraint(Z3_ast expr) ;
    void reset(); 
    std::string getAssignment(Z3_ast expr, size_t width);

  private:

    Z3Instance();
    Z3_model _m;
    Z3_context _ctx;
    Z3_ast _blocking_clause;
    thread_local static unsigned _pushed;
  };

} // namespace UrsaMajor

#endif
