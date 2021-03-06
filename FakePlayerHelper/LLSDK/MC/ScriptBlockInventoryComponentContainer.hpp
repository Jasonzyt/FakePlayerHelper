// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Scripting.hpp"
#include "ScriptContainer.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ScriptBlockInventoryComponentContainer : public ScriptContainer {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SCRIPTBLOCKINVENTORYCOMPONENTCONTAINER
public:
    ScriptBlockInventoryComponentContainer(class ScriptBlockInventoryComponentContainer const&) = delete;
    ScriptBlockInventoryComponentContainer() = delete;
#endif

public:
    /*0*/ virtual ~ScriptBlockInventoryComponentContainer();
    /*1*/ virtual class Scripting::Result<int> getEmptySlotsCount() const;
    /*2*/ virtual class Container* _tryGetContainer() const;
    /*3*/ virtual void _balanceTransaction(class ItemStack const&) const;
    MCAPI class ScriptBlockInventoryComponentContainer& operator=(class ScriptBlockInventoryComponentContainer const&);
    MCAPI static class Scripting::ClassBindingBuilder<class ScriptBlockInventoryComponentContainer> bind(struct Scripting::Version);

protected:

private:

};