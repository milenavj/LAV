#ifndef __FORMULAVECTOR_H
#define __FORMULAVECTOR_H

#include <vector>

using namespace std;

#include "Formula.h"

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVector
////////////////////////////////////////////////////////////////////////////////
namespace UrsaMajor {

class FormulaVector {

public:

  FormulaVector(size_t width);
  FormulaVector(unsigned int value, size_t width);
  FormulaVector(const FormulaVector &fv);

  ~FormulaVector();

  FormulaVector &operator=(unsigned int n);
  FormulaVector &operator=(const FormulaVector &fv);
  FormulaVector &operator=(Formula *f);

  void setFormulaAt(unsigned int i, Formula *f);
  Formula *operator[](unsigned int i) const;

  FormulaVector TruthValue() const;
  bool isGround() const;

  unsigned getUnsignedValue() const;
  bool getBooleanValue() const;

  FormulaVector operator~() const;
  FormulaVector operator&(const FormulaVector &fv) const;
  FormulaVector operator&(const unsigned long c) const;
  FormulaVector operator|(const FormulaVector &fv) const;
  FormulaVector operator^(const FormulaVector &fv) const;
  FormulaVector operator<<(unsigned int k) const;
  FormulaVector operator>>(unsigned int k) const;

  FormulaVector operator&&(const FormulaVector &fv) const;
  FormulaVector operator||(const FormulaVector &fv) const;

  FormulaVector &operator+=(const FormulaVector &fv);
  FormulaVector operator+(const FormulaVector &fv) const;
  FormulaVector operator*(const FormulaVector &fv) const;
  FormulaVector operator-(const FormulaVector &fv) const;

  FormulaVector operator<(const FormulaVector &fv) const;
  FormulaVector operator>(const FormulaVector &fv) const;
  FormulaVector operator<=(const FormulaVector &fv) const;
  FormulaVector operator>=(const FormulaVector &fv) const;
  FormulaVector operator==(const FormulaVector &fv) const;
  FormulaVector operator!=(const FormulaVector &fv) const;

  FormulaVector ite(const FormulaVector &fv1, const FormulaVector &fv2) const;

  unsigned int getWidth(void) const { return size; }
  ;
  unsigned int GetIndex(void) const { return m_nID; }
  ;

  static unsigned int getPropositionalVarsCount(void) {
    return m_iPropositionalVarCounter;
  }
  ;

  void print(void) const;

private:

  bool IsIndependent() { return (m_nID != -1); }
  ;
  void SetIsDependent() { m_nID = -1; }
  ;
  void SetIndependentIndex(unsigned int i) { m_nID = i; }
  ;

  Formula *doAnd(void);
  Formula *doOr(void);

  int m_nID;
  Formula **bitArray;
  unsigned int size;

  static unsigned int m_iPropositionalVarCounter;

};

} //end of namespace

#endif
