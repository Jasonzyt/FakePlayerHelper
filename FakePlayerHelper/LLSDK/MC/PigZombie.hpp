// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Actor.hpp"
#include "Zombie.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class PigZombie : public Zombie {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_PIGZOMBIE
public:
    class PigZombie& operator=(class PigZombie const&) = delete;
    PigZombie(class PigZombie const&) = delete;
    PigZombie() = delete;
#endif

public:
    /*0*/ virtual ~PigZombie();
    /*1*/ virtual bool isRuntimePredictedMovementEnabled() const;
    /*2*/ virtual void __unk_vfn_0();
    /*3*/ virtual class Vec3 getInterpolatedRidingOffset(float) const;
    /*4*/ virtual void __unk_vfn_1();
    /*5*/ virtual void normalTick();
    /*6*/ virtual float getRidingHeight();
    /*7*/ virtual void __unk_vfn_2();
    /*8*/ virtual void __unk_vfn_3();
    /*9*/ virtual float getCameraOffset() const;
    /*10*/ virtual void __unk_vfn_4();
    /*11*/ virtual void __unk_vfn_5();
    /*12*/ virtual void playerTouch(class Player&);
    /*13*/ virtual void __unk_vfn_6();
    /*14*/ virtual void __unk_vfn_7();
    /*15*/ virtual bool isDamageBlocked(class ActorDamageSource const&) const;
    /*16*/ virtual void __unk_vfn_8();
    /*17*/ virtual void __unk_vfn_9();
    /*18*/ virtual void __unk_vfn_10();
    /*19*/ virtual void __unk_vfn_11();
    /*20*/ virtual class Actor* findAttackTarget();
    /*21*/ virtual bool isValidTarget(class Actor*) const;
    /*22*/ virtual void adjustDamageAmount(int&) const;
    /*23*/ virtual void onTame();
    /*24*/ virtual void onFailedTame();
    /*25*/ virtual void vehicleLanded(class Vec3 const&, class Vec3 const&);
    /*26*/ virtual void onBounceStarted(class BlockPos const&, class Block const&);
    /*27*/ virtual float getPickRadius();
    /*28*/ virtual void awardKillScore(class Actor&, int);
    /*29*/ virtual class HashedString const& queryEntityRenderer() const;
    /*30*/ virtual struct ActorUniqueID getSourceUniqueID() const;
    /*31*/ virtual int getPortalWaitTime() const;
    /*32*/ virtual bool canChangeDimensions() const;
    /*33*/ virtual void __unk_vfn_12();
    /*34*/ virtual struct ActorUniqueID getControllingPlayer() const;
    /*35*/ virtual bool canBePulledIntoVehicle() const;
    /*36*/ virtual void __unk_vfn_13();
    /*37*/ virtual bool canSynchronizeNewEntity() const;
    /*38*/ virtual void __unk_vfn_14();
    /*39*/ virtual void __unk_vfn_15();
    /*40*/ virtual bool isWorldBuilder() const;
    /*41*/ virtual void __unk_vfn_16();
    /*42*/ virtual bool isAdventure() const;
    /*43*/ virtual bool canDestroyBlock(class Block const&) const;
    /*44*/ virtual void setAuxValue(int);
    /*45*/ virtual void stopSpinAttack();
    /*46*/ virtual void __unk_vfn_17();
    /*47*/ virtual void __unk_vfn_18();
    /*48*/ virtual void __unk_vfn_19();
    /*49*/ virtual bool _hurt(class ActorDamageSource const&, int, bool, bool);
    /*50*/ virtual void readAdditionalSaveData(class CompoundTag const&, class DataLoadHelper&);
    /*51*/ virtual void addAdditionalSaveData(class CompoundTag&);
    /*52*/ virtual void __unk_vfn_20();
    /*53*/ virtual void __unk_vfn_21();
    /*54*/ virtual void spawnAnim();
    /*55*/ virtual void aiStep();
    /*56*/ virtual bool checkSpawnRules(bool);
    /*57*/ virtual float getItemUseStartupProgress() const;
    /*58*/ virtual float getItemUseIntervalProgress() const;
    /*59*/ virtual void __unk_vfn_22();
    /*60*/ virtual bool isAlliedTo(class Mob*);
    /*61*/ virtual void __unk_vfn_23();
    /*62*/ virtual int getArmorValue();
    /*63*/ virtual void sendArmorDamage(class std::bitset<4> const&);
    /*64*/ virtual void onBorn(class Actor&, class Actor&);
    /*65*/ virtual int getAttackTime();
    /*66*/ virtual void __unk_vfn_24();
    /*67*/ virtual void _serverAiMobStep();
    /*68*/ virtual void __unk_vfn_25();
    MCAPI PigZombie(class ActorDefinitionGroup*, struct ActorDefinitionIdentifier const&, class OwnerPtrT<struct EntityRefTraits> const&);

protected:

private:
    MCAPI static class mce::UUID const SPEED_MODIFIER_ATTACK_UUID;

};