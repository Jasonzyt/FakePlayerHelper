// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Scripting.hpp"
#include "ScriptObject.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ScriptContainer : public ScriptObject {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SCRIPTCONTAINER
public:
    class ScriptContainer& operator=(class ScriptContainer const&) = delete;
    ScriptContainer(class ScriptContainer const&) = delete;
    ScriptContainer() = delete;
#endif

public:
    /*0*/ virtual ~ScriptContainer();
    /*1*/ virtual class Scripting::Result<int> getSize() const;
    /*2*/ virtual class Scripting::Result<int> getEmptySlotsCount() const;
    /*3*/ virtual class Scripting::Result<void> setItem(int, class ScriptItemStack const&) const;
    /*4*/ virtual class Scripting::Result<class Scripting::StrongTypedObjectHandle<class ScriptItemStack> > getItem(int) const;
    /*5*/ virtual class Scripting::Result<void> addItem(class ScriptItemStack const&) const;
    /*6*/ virtual class Scripting::Result<bool> transferItem(int, int, class ScriptContainer&) const;
    /*7*/ virtual class Scripting::Result<bool> swapItems(int, int, class ScriptContainer&) const;
    /*8*/ virtual class Container* _tryGetContainer() const = 0;
    /*9*/ virtual void _balanceTransaction(class ItemStack const&) const;
    MCAPI ScriptContainer(class Scripting::WeakLifetimeScope const&);
    MCAPI static class Scripting::ClassBindingBuilder<class ScriptContainer> bind(struct Scripting::Version);

protected:

private:

};