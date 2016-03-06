#ifndef __EXPRESSION_TYPES_H
#define __EXPRESSION_TYPES_H

namespace argo {
/**
* Legal types of expression tree nodes
*/
enum EXPRESSION_TYPE{
	//Variables
	EXPR_VARIABLE, 
	EXPR_FORMULA_VARIABLE,
	EXPR_METAVARIABLE,

	//Terms
	EXPR_CONSTANT,
	EXPR_FUNCTION, 
	EXPR_INT_NUMERAL,
	EXPR_RATIONAL_NUMERAL,
	EXPR_TERM_ITE,

	//Atoms
	EXPR_PREDICATE,
	EXPR_EQUALITY,
	EXPR_DISEQUALITY,
	EXPR_TOP, 
	EXPR_BOT,

	//Logical connectives
	EXPR_AND,
	EXPR_OR,
	EXPR_XOR,
	EXPR_IMPL,
	EXPR_IFF,
	EXPR_NOT,

	//Quantifiers
	EXPR_UNIVERSALQUANTIFIER,
	EXPR_EXISTENTIALQUANTIFIER,
	
	//Theory specific expressions
	
	//arrays
	EXPR_ARRAY_PARTIAL_EQUALITY
};

} //end of namespace

#endif

