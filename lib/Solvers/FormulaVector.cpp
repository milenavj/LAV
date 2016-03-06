#include <assert.h>
#include "solvers/solver-interfaces/FormulaVector.h"
#include "solvers/solver-interfaces/FormulaFactory.h"

namespace UrsaMajor {

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVector
////////////////////////////////////////////////////////////////////////////////


unsigned int FormulaVector::m_iPropositionalVarCounter;

// -------------------------------------------------------------------


FormulaVector::FormulaVector(const FormulaVector &fv) {
  size=fv.getWidth();
  bitArray = new Formula* [size];
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i] = fv.bitArray[i];
    fv.bitArray[i]->IncRefCount();
  }
  m_nID = fv.m_nID;
}


// -------------------------------------------------------------------


// This constructor should be called ONLY for independent symbolic variables
FormulaVector::FormulaVector(size_t width) {
  size = width;
  bitArray = new Formula* [size];

  SetIndependentIndex(m_iPropositionalVarCounter+1);

  for (unsigned int i = 0; i < size; i++) { 
    Formula *f = FormulaFactory::Instance()->Get(new FormulaVar(++m_iPropositionalVarCounter));
    Formula::NEW++;
    bitArray[i] = f;
    f->IncRefCount();
  }
}


// -------------------------------------------------------------------


FormulaVector::FormulaVector(unsigned int value, size_t width)  {
  unsigned int i, j;
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  size = width;
  bitArray = new Formula* [size];
  for (i = 0, j = value; i < size; i++, j /= 2)
    if (j % 2) {
       bitArray[size-i-1] = T;
       T->IncRefCount();
    }
    else {
       bitArray[size-i-1] = NT;
       NT->IncRefCount();
    }

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);

  SetIsDependent(); 
}


// -------------------------------------------------------------------


FormulaVector::~FormulaVector() {
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i]->DecRefCount();
    FormulaFactory::Instance()->Remove(bitArray[i]);
  }

  delete [] bitArray;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::TruthValue() const {
  FormulaVector result(0,1), r(0,1);

  r = (*this)[size-1];
  result = r;

  for(int i=size-2;i>=0;i--) {
    r = (*this)[i];
    result = result & r;
  }
 
  return result;
}


// -------------------------------------------------------------------

bool FormulaVector::isGround() const {
  for (unsigned int i = 0; i < getWidth(); i++)
    if (bitArray[i]->GetType() != CONST) 
      return false;
  return true;
} 


// -------------------------------------------------------------------


unsigned FormulaVector::getUnsignedValue() const {
  unsigned n=0;
  for (int i = getWidth()-1; i >= 0 ; i--)  {
    n <<= 1;
    if (bitArray[i]->GetType() == CONST) 
      if (((FormulaConst*)(bitArray[i]))->GetValue() == true) 
        n |= 1;
  }
  return n;
}

// -------------------------------------------------------------------

bool FormulaVector::getBooleanValue() const {
  assert(size == 1);
  return ((FormulaConst*)(bitArray[0]))->GetValue();
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator = (unsigned int value) {
  unsigned int i, j;
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (i = 0, j = value; i < size; i++, j /= 2)
    if (j % 2) {
       bitArray[size-i-1] = T;
       T->IncRefCount();
    }
    else {
       bitArray[size-i-1] = NT;
       NT->IncRefCount();
    }

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);

  SetIsDependent(); 

  return *this;
}


 
// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator = (const FormulaVector &fv) {
  if (size != fv.getWidth())  {
    delete [] bitArray;
    size = fv.getWidth();
    bitArray = new Formula* [size];
  }

  for (unsigned int i = 0; i < fv.getWidth(); i++)
    setFormulaAt(i, fv.bitArray[i]);

  m_nID = fv.m_nID;
  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator = (Formula *f) {
  assert(size == 1);
  setFormulaAt(0, f);
  SetIsDependent(); 
  return *this;
}


// -------------------------------------------------------------------


void FormulaVector::setFormulaAt(unsigned int i, Formula *f) {
  Formula *tmp = bitArray[i];
  bitArray[i] = f;
  f->IncRefCount();
  tmp->DecRefCount();
  FormulaFactory::Instance()->Remove(tmp);
}


// -------------------------------------------------------------------


Formula* FormulaVector::operator [] (unsigned int i) const {
  return bitArray[i];
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator ~ () const {

  FormulaVector notFormulaVector(0,size);

  Formula *tmp;
  for (unsigned int i = 0; i < size; i++) {
    tmp = Formula::makeNot(bitArray[i]);
    notFormulaVector.setFormulaAt(i, tmp);
  }

  return notFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator & (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector res(0,size);

  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, f);
  }
  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator & (const unsigned long c) const {
  FormulaVector res(0,size);
  res = c;
  return res & *this;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator | (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector res(0,size);

  Formula *tmp;
  for (unsigned int i = 0; i < size; i++) {
    tmp = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, tmp);
  }

  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator ^ (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector res(0,size);

  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeXor(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, f);
  }

  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator << (unsigned int k) const {
  FormulaVector shlFormulaVector(0,size);
  assert(k >= 0);

  for (unsigned int i = k; i < size; i++)
    shlFormulaVector.setFormulaAt(i - k, bitArray[i]);

  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  for (unsigned int i = 0; i < k; i++)
    shlFormulaVector.setFormulaAt(size - k + i, f);

  return shlFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator >> (unsigned int k) const {
  FormulaVector shrFormulaVector(0,size);
  assert(k >= 0 && k < size);

  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  for (unsigned int i = 0; i < k; i++)
    shrFormulaVector.setFormulaAt(i, f);

  for (unsigned int i = k; i < size; i++)
    shrFormulaVector.setFormulaAt(i, bitArray[i - k]);

  return shrFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator && (const FormulaVector &fv) const  { 
  assert(size==1 && fv.getWidth()==1);
  FormulaVector res(0,1);
  res.setFormulaAt(0, Formula::makeAnd(bitArray[0], fv.bitArray[0]));
  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator || (const FormulaVector &fv) const  { 
  assert(size == fv.getWidth() && size == 1);
  FormulaVector res(0,size);
  res.setFormulaAt(0, Formula::makeOr(bitArray[0], fv.bitArray[0]));
  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator * (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector res(0, 2*size);

  for (unsigned int i = 0; i < size; i++) {
    Formula *f = Formula::makeAnd(bitArray[i], fv.bitArray[fv.getWidth() - 1]);
    res.setFormulaAt(i + size, f);
  }

  FormulaVector tmp_p(0, 2*size);
  for (int j = size - 1; j >= 0; j--)
    tmp_p.setFormulaAt(size + j, bitArray[j]);

  for (int j = fv.getWidth() - 2; j >= 0; j--) {

    FormulaVector tmp = tmp_p << (fv.getWidth() - 1 - j);
    Formula *f;
    for (unsigned int i = 0; i < tmp.getWidth(); i++) {
      f = Formula::makeAnd(tmp.bitArray[i], fv.bitArray[j]);
      tmp.setFormulaAt(i, f);
    }
    res += tmp;
  }

  FormulaVector result(0,size);
  for (unsigned j = 0; j < size; j++)
    result.setFormulaAt(j, res.bitArray[size+j]);

  return result;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator += (const FormulaVector &fv) {
  assert(size == fv.getWidth());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *sumF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    andF = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    orF = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    c->IncRefCount();
    sumF = Formula::makeXor(xorF, c);

    setFormulaAt(i, sumF);

    c->DecRefCount();
    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
  }

  FormulaFactory::Instance()->Remove(c);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator + (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector plusFormulaVector(0,fv.getWidth());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *sumF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    andF = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    orF = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    c->IncRefCount();
    sumF = Formula::makeXor(xorF, c);

    plusFormulaVector.setFormulaAt(i, sumF);

    c->DecRefCount();
    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
  }

  FormulaFactory::Instance()->Remove(c);

  return plusFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator - (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector minusFormulaVector(0,fv.getWidth());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  c->IncRefCount();
  Formula::NEW++;
  Formula *difF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    difF = Formula::makeXor(xorF, c);

    minusFormulaVector.setFormulaAt(i, difF);

    c->DecRefCount();

    Formula *notF = Formula::makeNot(bitArray[i]);
    notF->IncRefCount();

    andF = Formula::makeAnd(notF, fv.bitArray[i]);
    orF = Formula::makeOr(notF, fv.bitArray[i]);

    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
    c->IncRefCount();

    notF->DecRefCount();
    FormulaFactory::Instance()->Remove(notF);
  }

  c->DecRefCount();
  FormulaFactory::Instance()->Remove(c);

  return minusFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::ite(const FormulaVector &fv1, const FormulaVector &fv2) const {
  assert(size == 1);
  assert(fv1.getWidth() == fv2.getWidth());

  FormulaVector result(0,fv1.getWidth());
  Formula *r1,*r2,*r3;
  for(unsigned i=0; i < fv1.getWidth(); i++) {
    if(fv1.bitArray[i]==fv2.bitArray[i])
      result.setFormulaAt(i,fv1.bitArray[i]);
    else {
      r1=Formula::makeNot(bitArray[0]);
      r2=Formula::makeOr(r1,fv1.bitArray[i]);
      r3=Formula::makeOr(bitArray[0],fv2.bitArray[i]);
      result.setFormulaAt(i,Formula::makeAnd(r2,r3));
    }
  }

  return result;  
}


// -------------------------------------------------------------------


Formula* FormulaVector::doAnd(void) {
  Formula *f = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (unsigned int i = 0; i < size; i++)
    f = Formula::makeAnd(f, bitArray[i]);

  return f;
}


// -------------------------------------------------------------------


Formula* FormulaVector::doOr(void) {
  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;

  for (unsigned int i = 0; i < size; i++)
    f = Formula::makeOr(f, bitArray[i]);

  return f;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator < (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector result(0,1), r1(0,1), r2(0,1);

  r2 = (*this)[size-1];
  r1 = fv[size-1];
  result = (r1 & ~r2);

  for(int i=size-2;i>=0;i--) {
    r2 = (*this)[i];
    r1 = fv[i];
    result = (r1 & ~r2) | (result & (r1==r2));
  }
 
  return result;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator > (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector result(0,1), r1(0,1), r2(0,1);

  r1 = (*this)[size-1];
  r2 = fv[size-1];
  result = (r1 & ~r2);

  for(int i=size-2;i>=0;i--) {
    r1 = (*this)[i];
    r2 = fv[i];
    result = (r1 & ~r2) | (result & (r1==r2));
  }
  return result;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator <= (const FormulaVector &fv) const {
  return ~(*this>fv);
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator >= (const FormulaVector &fv) const {
  return ~(*this<fv);
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator == (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector result(0,1), r(0,1);

  result.setFormulaAt(0,Formula::makeEquiv(bitArray[0], fv.bitArray[0]));
  for (unsigned int i = 1; i < size; i++) {
    r.setFormulaAt(0,Formula::makeEquiv(bitArray[i], fv.bitArray[i]));
    result = result & r;
  }

  return result;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator != (const FormulaVector &fv) const {
  assert(size == fv.getWidth());
  FormulaVector result(0,1), r(0,1);

  result.setFormulaAt(0,Formula::makeXor(bitArray[0], fv.bitArray[0]));
  for (unsigned int i = 1; i < size; i++) {
    r.setFormulaAt(0,Formula::makeXor(bitArray[i], fv.bitArray[i]));
    result = result | r;
  }

  return result;
}


// -------------------------------------------------------------------


void FormulaVector::print(void) const {
  for (unsigned int i = 0; i < size; i++) {
    cout << i+1 << ". ";
    bitArray[i]->print();
    cout << endl;
  }
  cout << endl;
}


// -------------------------------------------------------------------


} //end of namespace