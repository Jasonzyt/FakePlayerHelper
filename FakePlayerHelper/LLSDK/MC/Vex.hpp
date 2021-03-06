// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Actor.hpp"
#include "Monster.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class Vex : public Monster {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_VEX
public:
    class Vex& operator=(class Vex const&) = delete;
    Vex(class Vex const&) = delete;
    Vex() = delete;
#endif

public:
    /*0*/ virtual void reloadHardcoded(enum Actor::InitializationMethod, class VariantParameterList const&);
    /*1*/ virtual void initializeComponents(enum Actor::InitializationMethod, class VariantParameterList const&);
    /*2*/ virtual ~Vex();
    /*3*/ virtual bool isRuntimePredictedMovementEnabled() const;
    /*4*/ virtual void __unk_vfn_0();
    /*5*/ virtual class Vec3 getInterpolatedRidingOffset(float) const;
    /*6*/ virtual void __unk_vfn_1();
    /*7*/ virtual float getRidingHeight();
    /*8*/ virtual bool isInWall() const;
    /*9*/ virtual void __unk_vfn_2();
    /*10*/ virtual void __unk_vfn_3();
    /*11*/ virtual float getCameraOffset() const;
    /*12*/ virtual void __unk_vfn_4();
    /*13*/ virtual void __unk_vfn_5();
    /*14*/ virtual void playerTouch(class Player&);
    /*15*/ virtual void __unk_vfn_6();
    /*16*/ virtual void __unk_vfn_7();
    /*17*/ virtual bool isDamageBlocked(class ActorDamageSource const&) const;
    /*18*/ virtual void __unk_vfn_8();
    /*19*/ virtual void __unk_vfn_9();
    /*20*/ virtual void __unk_vfn_10();
    /*21*/ virtual void __unk_vfn_11();
    /*22*/ virtual bool isValidTarget(class Actor*) const;
    /*23*/ virtual void adjustDamageAmount(int&) const;
    /*24*/ virtual void onTame();
    /*25*/ virtual void onFailedTame();
    /*26*/ virtual void vehicleLanded(class Vec3 const&, class Vec3 const&);
    /*27*/ virtual void onBounceStarted(class BlockPos const&, class Block const&);
    /*28*/ virtual float getPickRadius();
    /*29*/ virtual void awardKillScore(class Actor&, int);
    /*30*/ virtual class HashedString const& queryEntityRenderer() const;
    /*31*/ virtual struct ActorUniqueID getSourceUniqueID() const;
    /*32*/ virtual int getPortalWaitTime() const;
    /*33*/ virtual bool canChangeDimensions() const;
    /*34*/ virtual void __unk_vfn_12();
    /*35*/ virtual struct ActorUniqueID getControllingPlayer() const;
    /*36*/ virtual bool canPickupItem(class ItemStack const&) const;
    /*37*/ virtual bool canBePulledIntoVehicle() const;
    /*38*/ virtual void __unk_vfn_13();
    /*39*/ virtual bool canSynchronizeNewEntity() const;
    /*40*/ virtual void __unk_vfn_14();
    /*41*/ virtual void __unk_vfn_15();
    /*42*/ virtual bool isWorldBuilder() const;
    /*43*/ virtual void __unk_vfn_16();
    /*44*/ virtual bool isAdventure() const;
    /*45*/ virtual bool canDestroyBlock(class Block const&) const;
    /*46*/ virtual void setAuxValue(int);
    /*47*/ virtual void stopSpinAttack();
    /*48*/ virtual void __unk_vfn_17();
    /*49*/ virtual void __unk_vfn_18();
    /*50*/ virtual void __unk_vfn_19();
    /*51*/ virtual void readAdditionalSaveData(class CompoundTag const&, class DataLoadHelper&);
    /*52*/ virtual void addAdditionalSaveData(class CompoundTag&);
    /*53*/ virtual void __unk_vfn_20();
    /*54*/ virtual void __unk_vfn_21();
    /*55*/ virtual void spawnAnim();
    /*56*/ virtual void applyFinalFriction(float, bool);
    /*57*/ virtual void aiStep();
    /*58*/ virtual bool checkSpawnRules(bool);
    /*59*/ virtual int getItemUseDuration() const;
    /*60*/ virtual float getItemUseStartupProgress() const;
    /*61*/ virtual float getItemUseIntervalProgress() const;
    /*62*/ virtual void __unk_vfn_22();
    /*63*/ virtual bool isAlliedTo(class Mob*);
    /*64*/ virtual void __unk_vfn_23();
    /*65*/ virtual void sendArmorDamage(class std::bitset<4> const&);
    /*66*/ virtual void onBorn(class Actor&, class Actor&);
    /*67*/ virtual int getAttackTime();
    /*68*/ virtual void __unk_vfn_24();
    /*69*/ virtual void _serverAiMobStep();
    /*70*/ virtual void __unk_vfn_25();
    /*
    inline void addAdditionalSaveData(class CompoundTag& a0){
        void (Vex::*rv)(class CompoundTag&);
        *((void**)&rv) = dlsym("?addAdditionalSaveData@Vex@@UEAAXAEAVCompoundTag@@@Z");
        return (this->*rv)(std::forward<class CompoundTag&>(a0));
    }
    inline void readAdditionalSaveData(class CompoundTag const& a0, class DataLoadHelper& a1){
        void (Vex::*rv)(class CompoundTag const&, class DataLoadHelper&);
        *((void**)&rv) = dlsym("?readAdditionalSaveData@Vex@@UEAAXAEBVCompoundTag@@AEAVDataLoadHelper@@@Z");
        return (this->*rv)(std::forward<class CompoundTag const&>(a0), std::forward<class DataLoadHelper&>(a1));
    }
    */
    MCAPI Vex(class ActorDefinitionGroup*, struct ActorDefinitionIdentifier const&, class OwnerPtrT<struct EntityRefTraits> const&);

protected:

private:

};