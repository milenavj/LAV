#include "expression/output/OutputFormater.h"

namespace argo {

int printing_precedence[] = { 7, //EXPR_VARIABLE,
                              7, //EXPR_FORMULA_VARIABLE,
                              7, //EXPR_METAVARIABLE,
                              7, //EXPR_CONSTANT,
                              6, //EXPR_FUNCTION,
                              7, //EXPR_INT_NUMERAL,
                              7, //EXPR_RATIONAL_NUMERAL,
                              7, //EXPR_ITE,
                              5, //EXPR_PREDICATE,
                              5, //EXPR_EQUALITY,
                              5, //EXPR_DISEQUALITY,
                              5, //EXPR_TOP,
                              5, //EXPR_BOT,
                              3, //EXPR_AND,
                              3, //EXPR_OR,
                              3, //EXPR_XOR,
                              1, //EXPR_IMPLICATION,
                              1, //EXPR_EQUIVALENCE,
                              4, //EXPR_NOT,
                              0, //EXPR_UNIVERSALQUANTIFIER,
                              0  //EXPR_EXISTENTIALQUANTIFIER,
};

} //end of namespace