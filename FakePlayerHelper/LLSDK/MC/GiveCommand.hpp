// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class GiveCommand {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_GIVECOMMAND
public:
    class GiveCommand& operator=(class GiveCommand const&) = delete;
    GiveCommand(class GiveCommand const&) = delete;
    GiveCommand() = delete;
#endif

public:
    /*0*/ virtual ~GiveCommand();
    /*1*/ virtual void execute(class CommandOrigin const&, class CommandOutput&) const;
    MCAPI static void setup(class CommandRegistry&);

protected:

private:

};