// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class PlayAnimationCommand {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_PLAYANIMATIONCOMMAND
public:
    class PlayAnimationCommand& operator=(class PlayAnimationCommand const&) = delete;
    PlayAnimationCommand(class PlayAnimationCommand const&) = delete;
    PlayAnimationCommand() = delete;
#endif

public:
    /*0*/ virtual ~PlayAnimationCommand();
    /*1*/ virtual void execute(class CommandOrigin const&, class CommandOutput&) const;
    /*
    inline  ~PlayAnimationCommand(){
         (PlayAnimationCommand::*rv)();
        *((void**)&rv) = dlsym("??1PlayAnimationCommand@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI static void setup(class CommandRegistry&);

protected:

private:
    MCAPI static enum MolangVersion const mStopExpressionVersion;

};