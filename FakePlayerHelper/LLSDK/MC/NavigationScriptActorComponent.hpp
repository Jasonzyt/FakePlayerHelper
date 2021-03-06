// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Scripting.hpp"
#include "ScriptActorComponent.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class NavigationScriptActorComponent : public ScriptActorComponent {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_NAVIGATIONSCRIPTACTORCOMPONENT
public:
    class NavigationScriptActorComponent& operator=(class NavigationScriptActorComponent const&) = delete;
    NavigationScriptActorComponent(class NavigationScriptActorComponent const&) = delete;
    NavigationScriptActorComponent() = delete;
#endif

public:
    /*0*/ virtual ~NavigationScriptActorComponent();
    /*1*/ virtual class Scripting::Result<bool> getIsAmphibious() const;
    /*2*/ virtual class Scripting::Result<bool> getAvoidSun() const;
    /*3*/ virtual class Scripting::Result<bool> getCanPassDoors() const;
    /*4*/ virtual class Scripting::Result<bool> getCanOpenDoors() const;
    /*5*/ virtual class Scripting::Result<bool> getCanOpenIronDoors() const;
    /*6*/ virtual class Scripting::Result<bool> getCanBreakDoors() const;
    /*7*/ virtual class Scripting::Result<bool> getAvoidWater() const;
    /*8*/ virtual class Scripting::Result<bool> getAvoidDamageBlocks() const;
    /*9*/ virtual class Scripting::Result<bool> getCanFloat() const;
    /*10*/ virtual class Scripting::Result<bool> getCanSink() const;
    /*11*/ virtual class Scripting::Result<bool> getCanPathOverWater() const;
    /*12*/ virtual class Scripting::Result<bool> getCanPathOverLava() const;
    /*13*/ virtual class Scripting::Result<bool> getCanWalkInLava() const;
    /*14*/ virtual class Scripting::Result<bool> getAvoidPortals() const;
    /*15*/ virtual class Scripting::Result<bool> getCanWalk() const;
    /*16*/ virtual class Scripting::Result<bool> getCanSwim() const;
    /*17*/ virtual class Scripting::Result<bool> getCanBreach() const;
    /*18*/ virtual class Scripting::Result<bool> getCanJump() const;
    /*19*/ virtual class Scripting::Result<bool> getCanPathFromAir() const;

protected:

private:

};