#include "expression/expressions/NumeralNode.h"

namespace argo {

int IntNumeralNode::compare(const ExpressionNode& e) const {
    if (this == &e)
	return 0;
    if (GetType() < e.GetType())
	return -1;
    else if (GetType() > e.GetType())
	return 1;
    const IntNumeralNode& n = (IntNumeralNode&)e;

    if (_value > n._value)
	return 1;
    else if (_value < n._value)
	return -1;
    return 0;
}

int RationalNumeralNode::compare(const ExpressionNode& e) const {
    if (this == &e)
	return 0;
    if (GetType() < e.GetType())
	return -1;
    else if (GetType() > e.GetType())
	return 1;
    const RationalNumeralNode& n = (RationalNumeralNode&)e;

    if (_value > n._value)
	return 1;
    else if (_value < n._value)
	return -1;
    return 0;
}

} //end of namespace