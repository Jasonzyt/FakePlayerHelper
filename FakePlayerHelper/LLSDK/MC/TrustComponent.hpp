// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class TrustComponent {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_TRUSTCOMPONENT
public:
    class TrustComponent& operator=(class TrustComponent const&) = delete;
    TrustComponent(class TrustComponent const&) = delete;
#endif

public:
    MCAPI TrustComponent();
    MCAPI TrustComponent(class TrustComponent&&);
    MCAPI void addAdditionalSaveData(class CompoundTag&);
    MCAPI void assignTrustedPlayer(struct ActorUniqueID);
    MCAPI class std::unordered_set<struct ActorUniqueID, struct std::hash<struct ActorUniqueID>, struct std::equal_to<struct ActorUniqueID>, class std::allocator<struct ActorUniqueID> > const& getTrustedPlayerIDs() const;
    MCAPI void initFromDefinition(class Actor&);
    MCAPI class TrustComponent& operator=(class TrustComponent&&);
    MCAPI void readAdditionalSaveData(class Actor&, class CompoundTag const&, class DataLoadHelper&);

protected:

private:

};