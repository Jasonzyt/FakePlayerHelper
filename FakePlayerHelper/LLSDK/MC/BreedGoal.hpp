// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class BreedGoal {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_BREEDGOAL
public:
    class BreedGoal& operator=(class BreedGoal const&) = delete;
    BreedGoal(class BreedGoal const&) = delete;
    BreedGoal() = delete;
#endif

public:
    /*0*/ virtual ~BreedGoal();
    /*1*/ virtual bool canUse();
    /*2*/ virtual bool canContinueToUse();
    /*3*/ virtual void __unk_vfn_0();
    /*4*/ virtual void __unk_vfn_1();
    /*5*/ virtual void stop();
    /*6*/ virtual void tick();
    /*7*/ virtual void appendDebugInfo(std::string&) const;
    MCAPI BreedGoal(class Mob&, float);

protected:

private:
    MCAPI void _breed();
    MCAPI bool _isCloseEnoughToBreed(class Vec3, class Vec2, class Vec3, class Vec2) const;
    MCAPI class Mob* _moveToFreePartner() const;

};