//===-- LMerge.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LMERGE_H
#define LMERGE_H

#include "lav/Internal/LTypes.h"
#include <vector>
namespace lav{

class LBlock;

class LMerge 
{
public:
  LMerge() : _Merged(false), _FMerged(NULL) {}

  inline const vpBlock& GetMerged ()      const {return _vMerged;}
  inline LBlock*        MergedInto()      const {return _FMerged;}
  inline bool           IsMerged  ()      const {return _Merged;}
  inline void           SetMerged (LBlock* lb)  {_Merged = true; _FMerged = lb;}
  inline void           AddMerged (LBlock* lb)  {_vMerged.push_back(lb);}
  inline bool           HasMerged ()      const {return _vMerged.size() > 0;}
  inline bool           NumMerged ()      const {return _vMerged.size();}
  inline LBlock*        LastMerged()      const {return _vMerged[_vMerged.size()-1];}
  inline const vpBlock& BlocksMerged()    const {return _vMerged;}

private:
  bool                          _Merged;//da li je ovaj blok nekome nakacen
  LBlock*                       _FMerged;//kome je nakacen
  vpBlock                       _vMerged;//da li je ovom bloku neko drugi nakacen i ko sve
};

}//end of namespace

#endif

