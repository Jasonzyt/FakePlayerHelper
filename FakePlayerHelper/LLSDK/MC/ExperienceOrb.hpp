// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Actor.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ExperienceOrb : public Actor {

#define AFTER_EXTRA
// Add Member There
public:
enum DropType;

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_EXPERIENCEORB
public:
    class ExperienceOrb& operator=(class ExperienceOrb const&) = delete;
    ExperienceOrb(class ExperienceOrb const&) = delete;
    ExperienceOrb() = delete;
#endif

public:
    /*0*/ virtual void reloadHardcoded(enum Actor::InitializationMethod, class VariantParameterList const&);
    /*1*/ virtual ~ExperienceOrb();
    /*2*/ virtual bool isRuntimePredictedMovementEnabled() const;
    /*3*/ virtual void __unk_vfn_0();
    /*4*/ virtual float getInterpolatedHeadRot(float) const;
    /*5*/ virtual float getInterpolatedBodyYaw(float) const;
    /*6*/ virtual class Vec3 getInterpolatedRidingOffset(float) const;
    /*7*/ virtual void __unk_vfn_1();
    /*8*/ virtual void normalTick();
    /*9*/ virtual float getRidingHeight();
    /*10*/ virtual void __unk_vfn_2();
    /*11*/ virtual void __unk_vfn_3();
    /*12*/ virtual float getCameraOffset() const;
    /*13*/ virtual float getShadowRadius() const;
    /*14*/ virtual void __unk_vfn_4();
    /*15*/ virtual void __unk_vfn_5();
    /*16*/ virtual void playerTouch(class Player&);
    /*17*/ virtual bool isPickable();
    /*18*/ virtual void __unk_vfn_6();
    /*19*/ virtual bool isSleeping() const;
    /*20*/ virtual void setSleeping(bool);
    /*21*/ virtual void __unk_vfn_7();
    /*22*/ virtual bool isBlocking() const;
    /*23*/ virtual bool isDamageBlocked(class ActorDamageSource const&) const;
    /*24*/ virtual void __unk_vfn_8();
    /*25*/ virtual bool isSurfaceMob() const;
    /*26*/ virtual void __unk_vfn_9();
    /*27*/ virtual void __unk_vfn_10();
    /*28*/ virtual void __unk_vfn_11();
    /*29*/ virtual class Actor* findAttackTarget();
    /*30*/ virtual bool isValidTarget(class Actor*) const;
    /*31*/ virtual void adjustDamageAmount(int&) const;
    /*32*/ virtual void onTame();
    /*33*/ virtual void onFailedTame();
    /*34*/ virtual bool isJumping() const;
    /*35*/ virtual void vehicleLanded(class Vec3 const&, class Vec3 const&);
    /*36*/ virtual bool isInvulnerableTo(class ActorDamageSource const&) const;
    /*37*/ virtual void animateHurt();
    /*38*/ virtual void onBounceStarted(class BlockPos const&, class Block const&);
    /*39*/ virtual float getPickRadius();
    /*40*/ virtual void awardKillScore(class Actor&, int);
    /*41*/ virtual enum ArmorMaterialType getArmorMaterialTypeInSlot(enum ArmorSlot) const;
    /*42*/ virtual enum ArmorTextureType getArmorMaterialTextureTypeInSlot(enum ArmorSlot) const;
    /*43*/ virtual float getArmorColorInSlot(enum ArmorSlot, int) const;
    /*44*/ virtual void setEquippedSlot(enum EquipmentSlot, class ItemStack const&);
    /*45*/ virtual class HashedString const& queryEntityRenderer() const;
    /*46*/ virtual struct ActorUniqueID getSourceUniqueID() const;
    /*47*/ virtual bool canFreeze() const;
    /*48*/ virtual int getPortalWaitTime() const;
    /*49*/ virtual bool canChangeDimensions() const;
    /*50*/ virtual void __unk_vfn_12();
    /*51*/ virtual struct ActorUniqueID getControllingPlayer() const;
    /*52*/ virtual bool canPickupItem(class ItemStack const&) const;
    /*53*/ virtual bool canBePulledIntoVehicle() const;
    /*54*/ virtual bool inCaravan() const;
    /*55*/ virtual void __unk_vfn_13();
    /*56*/ virtual bool canSynchronizeNewEntity() const;
    /*57*/ virtual void buildDebugInfo(std::string&) const;
    /*58*/ virtual int getDeathTime() const;
    /*59*/ virtual void swing();
    /*60*/ virtual void __unk_vfn_14();
    /*61*/ virtual void __unk_vfn_15();
    /*62*/ virtual float getYHeadRot() const;
    /*63*/ virtual bool isWorldBuilder() const;
    /*64*/ virtual void __unk_vfn_16();
    /*65*/ virtual bool isAdventure() const;
    /*66*/ virtual bool canDestroyBlock(class Block const&) const;
    /*67*/ virtual void setAuxValue(int);
    /*68*/ virtual void stopSpinAttack();
    /*69*/ virtual void __unk_vfn_17();
    /*70*/ virtual void __unk_vfn_18();
    /*71*/ virtual void updateEntitySpecificMolangVariables(class RenderParams&);
    /*72*/ virtual void __unk_vfn_19();
    /*73*/ virtual bool _hurt(class ActorDamageSource const&, int, bool, bool);
    /*74*/ virtual void readAdditionalSaveData(class CompoundTag const&, class DataLoadHelper&);
    /*75*/ virtual void addAdditionalSaveData(class CompoundTag&);
    /*76*/ virtual void _playStepSound(class BlockPos const&, class Block const&);
    /*77*/ virtual void __unk_vfn_20();
    /*78*/ virtual void doWaterSplashEffect();
    /*79*/ virtual void _onSizeUpdated();
    MCAPI ExperienceOrb(class ActorDefinitionGroup*, struct ActorDefinitionIdentifier const&, class OwnerPtrT<struct EntityRefTraits> const&);
    MCAPI int getIconIndex() const;
    MCAPI void setValue(int);
    MCAPI static void spawnOrbs(class BlockSource&, class Vec3 const&, int, int, enum ExperienceOrb::DropType, class Player*);
    MCAPI static void spawnOrbs(class BlockSource&, class Vec3 const&, int, enum ExperienceOrb::DropType, class Player*);

protected:

private:
    MCAPI void _handleMending(class Player&);
    MCAPI static int const LIFETIME;

};