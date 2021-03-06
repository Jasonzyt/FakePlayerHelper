// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "BaseMoveToBlockGoal.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class InspectBookshelfGoal : public BaseMoveToBlockGoal {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_INSPECTBOOKSHELFGOAL
public:
    class InspectBookshelfGoal& operator=(class InspectBookshelfGoal const&) = delete;
    InspectBookshelfGoal(class InspectBookshelfGoal const&) = delete;
    InspectBookshelfGoal() = delete;
#endif

public:
    /*0*/ virtual ~InspectBookshelfGoal();
    /*1*/ virtual bool canContinueToUse();
    /*2*/ virtual void __unk_vfn_0();
    /*3*/ virtual void tick();
    /*4*/ virtual void appendDebugInfo(std::string&) const;
    /*5*/ virtual void __unk_vfn_1();
    /*6*/ virtual void __unk_vfn_2();
    /*7*/ virtual bool isValidTarget(class BlockSource&, class BlockPos const&);
    /*8*/ virtual bool _canReach(class BlockPos const&);
    /*9*/ virtual unsigned __int64 _getRepathTime() const;
    /*10*/ virtual bool findTargetBlock();
    MCAPI InspectBookshelfGoal(class Mob&, float, int, int, int, float);

protected:

private:

};