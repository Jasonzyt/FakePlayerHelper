// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class DamageOverTimeSystem {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_DAMAGEOVERTIMESYSTEM
public:
    class DamageOverTimeSystem& operator=(class DamageOverTimeSystem const&) = delete;
    DamageOverTimeSystem(class DamageOverTimeSystem const&) = delete;
    DamageOverTimeSystem() = delete;
#endif

public:
    /*0*/ virtual ~DamageOverTimeSystem();
    /*1*/ virtual void __unk_vfn_0();
    /*2*/ virtual void tick(class EntityRegistry&);

protected:

private:

};