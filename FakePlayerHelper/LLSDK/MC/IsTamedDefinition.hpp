// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "JsonUtil.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

struct IsTamedDefinition {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_ISTAMEDDEFINITION
public:
    struct IsTamedDefinition& operator=(struct IsTamedDefinition const&) = delete;
    IsTamedDefinition(struct IsTamedDefinition const&) = delete;
    IsTamedDefinition() = delete;
#endif

public:
    MCAPI void initialize(class EntityContext&);
    MCAPI void uninitialize(class EntityContext&);

protected:

private:

};