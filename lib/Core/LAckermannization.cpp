//===-- LAckermannization.cpp ---------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/LAckermannization.h"
#include "expression/output/SMTFormater.h"

namespace lav {

static argo::SMTFormater SMTF;

aExp AbstrAckLeft(const aExp &e) {
  return ExpVar(LEFT + e.GetName(), fint_type, false);
}

aExp AbstrAckRight(const aExp &e) {
  return ExpVar(RIGHT + e.GetName(), fint_type, false);
}

aExp LAckermannization::FlattenLR(const aExp &a, saExp &lefts, saExp &rights,
                                  std::map<aExp, aExp> &_Acks) {
  vaExp eqs;
  aExp simplified = SimplifyLeftRight(a, eqs, lefts, rights, _Acks);

  return AddEqs(simplified, eqs);
}

//bez apstrakcija
//left(e0) =====>>>>> dodaje novu jednakost
//left(e0) = left(e0[0]) +- e0[1]
void LAckermannization::SimplifyLeftRight(caExp &e, saExp &eq) {

  if (e.GetArity() == 0)
    return;

  if (e.isLeft() || e.isRight()) {
    if (e[0].GetArity() == 0)
      return;

    if (e[0].isAdd()) {
      if (e.isRight())
        eq.insert(aExp::Equality(e, aExp::sub(ExpRight(e[0][0]), e[0][1])));
      if (e.isLeft())
        eq.insert(aExp::Equality(e, aExp::sub(ExpLeft(e[0][0]), e[0][1])));
    }

    if (e[0].isSub()) {
      if (e.isRight())
        eq.insert(aExp::Equality(e, aExp::add(ExpRight(e[0][0]), e[0][1])));
      if (e.isLeft())
        eq.insert(aExp::Equality(e, aExp::add(ExpLeft(e[0][0]), e[0][1])));
    }
    return;
  }

  for (unsigned i = 0; i < e.GetArity(); i++)
    (SimplifyLeftRight(e[i], eq));
}

//left(e0) =====>>>>>
//f1 = e0 /\ left(f1) = left(e0[0]) +- e0[1]
//ako je e0[0] == select
//f1 = e0 /\ f2 = e0[0] /\ left(f1) =left(f2) +- e0[1]
aExp LAckermannization::SimplifyLeftRight(const aExp &e, vaExp &eq,
                                          saExp &lefts, saExp &rights,
                                          std::map<aExp, aExp> &acks) {
  if (e.GetArity() == 0)
    return e;

  if (e.isLeft() || e.isRight()) {
    if (e[0].GetArity() == 0) {
      if (e.isLeft()) {
        lefts.insert(e[0]);
        return AbstrAckLeft(e[0]);
      } else {
        rights.insert(e[0]);
        return AbstrAckRight(e[0]);
      }
    }

    aExp newVar;

    if (acks.find(e[0]) == acks.end()) {
      std::string name = GetNextVariable();
      newVar = ExpVar(name, fpointer_type, false);
      //FIXME tip
      //    newVar = ExpVar(name, e[0].getIntType(), false);
      acks[e[0]] = newVar;
      aExp equality = aExp::Equality(newVar, ToPointer(e[0], e.getIntWidth()));
      eq.push_back(equality);
    } else {
      newVar = acks[e[0]];
    }
    //FIXME ako je u mapi onda umesto newvar uzmemo vrednost iz mape, ako nije u
    //mapi onda ubacimo u mapu
    aExp toinsert = aExp::TOP();

    aExp equality2;
    if (e[0].isAdd() || e[0].isGepi()) {
      if (!e[0][0].isSelect()) {
        if (e.isLeft()) {
          equality2 = aExp::Equality(
              aExp::sub(AbstrAckLeft(e[0][0]),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckLeft(newVar));
          toinsert = (e[0][0]);
        }
        if (e.isRight()) {
          equality2 = aExp::Equality(
              aExp::sub(AbstrAckRight(e[0][0]),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckRight(newVar));
          toinsert = (e[0][0]);
        }
      } else {
        aExp newVar2;
        if (acks.find(e[0][0]) == acks.end()) {
          /*
          //ako je e0[0] == select
          //ovo je stavljeno f1 = e0 /\
          //ovo stavljamo f2 = e0[0] /\
          //ovo je eq2 left(f1) =left(f2) += e0[1]
          */
          std::string name = GetNextVariable();
          newVar2 = ExpVar(name, fpointer_type, false);
          //FIXME tip
          //    newVar2 = ExpVar(name, e[0][0].getIntType(), false);
          //      aExp equality = aExp::Equality(newVar2, e[0][0]);
          aExp equality = aExp::Equality(
              newVar2, ToPointer(e[0][0], e[0][0].getIntWidth()));
          eq.push_back(equality);
        } else
          newVar2 = acks[e[0][0]];

        if (e.isLeft()) {
          equality2 = aExp::Equality(
              aExp::sub(AbstrAckLeft(newVar2),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckLeft(newVar));
          toinsert = (newVar2);
        }
        if (e.isRight()) {
          equality2 = aExp::Equality(
              aExp::sub(AbstrAckRight(newVar2),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckRight(newVar));
          toinsert = (newVar2);
        }
      }
    } else if (e[0].isSub()) //verovatno ovo ne moze
        {
      if (!e[0][0].isSelect()) {
        if (e.isLeft()) {
          equality2 = aExp::Equality(
              aExp::add(AbstrAckLeft(e[0][0]),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckLeft(newVar));
          toinsert = e[0][0];
        }
        if (e.isRight()) {
          equality2 = aExp::Equality(
              aExp::add(AbstrAckRight(e[0][0]),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckRight(newVar));
          toinsert = e[0][0];
        }
      } else {
        /*
        //ako je e0[0] == select
        //ovo je stavljeno f1 = e0 /\
        //ovo stavljamo f2 = e0[0] /\
        //ovo je eq2 left(f1) =left(f2) += e0[1]
        */
        aExp newVar2;
        if (acks.find(e[0][0]) == acks.end()) {
          //FIXME tip!!!!!!!!!!!!
          std::string name = GetNextVariable();
          newVar2 = ExpVar(name, fint_type, false);
          //          newVar2 = ExpVar(name, e[0][0].getIntType(), false);
          //	  aExp equality = aExp::Equality(newVar2, e[0][0]);
          aExp equality =
              aExp::Equality(newVar2, ToInt(e[0][0], e[0][0].getIntWidth()));
          eq.push_back(equality);
        } else
          newVar2 = acks[e[0][0]];

        if (e.isLeft()) {
          equality2 = aExp::Equality(
              aExp::add(AbstrAckLeft(newVar2),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckLeft(newVar));
          toinsert = newVar2;
        }
        if (e.isRight()) {
          equality2 = aExp::Equality(
              aExp::add(AbstrAckRight(newVar2),
                        ToInt(e[0][1], GetBitWidth(fpointer_type))),
              AbstrAckRight(newVar));
          toinsert = newVar2;
        }
      }
    }
    //    else assert(0); //ovde se dodje za select ili store ili neku
    // funkciju!!!

    if (!equality2.IsNULL())
      eq.push_back(equality2);

    if (e.isLeft()) {
      if (toinsert != aExp::TOP())
        lefts.insert(toinsert);
      lefts.insert(newVar);
      return AbstrAckLeft(newVar);
    }
    if (e.isRight()) {
      if (toinsert != aExp::TOP())
        rights.insert(toinsert);
      rights.insert(newVar);
      return AbstrAckRight(newVar);
    }
  }

  vaExp operands;
  for (unsigned i = 0; i < e.GetArity(); i++)
    operands.push_back(SimplifyLeftRight(e[i], eq, lefts, rights, acks));

  bool bb;
  return MakeExpression(e, operands, bb);

}

//ovo je kada se dodaje u solver
void LAckermannization::GlobalAckermannization(const aExp &cond, aExp &r_cond,
                                               saExp &ls, saExp &rs,
                                               std::map<aExp, aExp> &_Acks) {
  r_cond = cond;
  Ackermannize(r_cond, ls, rs, _Acks);
}

void LAckermannization::GlobalAckermannization(const aExp &a, const aExp &b,
                                               aExp &result_a, aExp &result_b,
                                               saExp &ls, saExp &rs,
                                               std::map<aExp, aExp> &_Acks) {
  std::map<aExp, aExp> acks = _Acks;
  result_b = b;
  result_a = a;
  AckermannizeWithoutAbstraction(result_a, result_b, acks, ls, rs);
}

//akrmanizuje izraz koji treba da se doda u solver
void LAckermannization::Ackermannize(aExp &cond, saExp &ls, saExp &rs,
                                     std::map<aExp, aExp> &_Acks) {
  if ((cond.GetArity() == 0))
    return;

  aExp flattened_cond = FlattenLR(cond, ls, rs, _Acks);
  ;
  if ((ls.size() == 0) && (rs.size() == 0))
    return;
  cond = flattened_cond;
  return;
}

void LAckermannization::AckermannizeWithoutAbstraction(
    aExp &a, aExp &b, std::map<aExp, aExp> &_Acks, saExp &ls, saExp &rs) {
  vaExp implications;

  if ((a.GetArity() == 0) && (b.GetArity() == 0))
    return;
  aExp flattened_a = FlattenLR(a, ls, rs, _Acks);
  vaExp eqs_b;
  aExp flattened_b = SimplifyLeftRight(b, eqs_b, ls, rs, _Acks);
  flattened_a = AddEqs(flattened_a, eqs_b);

  if ((ls.size() == 0) && (rs.size() == 0)) {
    return;
  }

  a = (flattened_a);
  b = (flattened_b);
  return;

}

//Akermanizuje a/\b nezavisno a nezavisno b
//dodaje sve sto je za akermanizaciju u novo a
//tako da moze da se proverava a/\b i a/\neg b
//ovo je ako je solver prazan pa mu se posalje
//na proveru a/\b
void LAckermannization::Ackermannize(aExp &a, aExp &b, saExp &ls, saExp &rs) {
  std::map<aExp, aExp> acks;
  AckermannizeWithoutAbstraction(a, b, acks, ls, rs);
  return;
}

}
