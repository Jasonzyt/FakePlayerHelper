// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "BlockActor.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class EndGatewayBlockActor : public BlockActor {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_ENDGATEWAYBLOCKACTOR
public:
    class EndGatewayBlockActor& operator=(class EndGatewayBlockActor const&) = delete;
    EndGatewayBlockActor(class EndGatewayBlockActor const&) = delete;
    EndGatewayBlockActor() = delete;
#endif

public:
    /*0*/ virtual ~EndGatewayBlockActor();
    /*1*/ virtual void load(class Level&, class CompoundTag const&, class DataLoadHelper&);
    /*2*/ virtual bool save(class CompoundTag&) const;
    /*3*/ virtual void saveBlockData(class CompoundTag&, class BlockSource&) const;
    /*4*/ virtual void loadBlockData(class CompoundTag const&, class BlockSource&, class DataLoadHelper&);
    /*5*/ virtual void onCustomTagLoadDone(class BlockSource&);
    /*6*/ virtual void tick(class BlockSource&);
    /*7*/ virtual void onChanged(class BlockSource&);
    /*8*/ virtual void onPlace(class BlockSource&);
    /*9*/ virtual void __unk_vfn_0();
    /*10*/ virtual void onRemoved(class BlockSource&);
    /*11*/ virtual void triggerEvent(int, int);
    /*12*/ virtual void clearCache();
    /*13*/ virtual void __unk_vfn_1();
    /*14*/ virtual float getShadowRadius(class BlockSource&) const;
    /*15*/ virtual bool hasAlphaLayer() const;
    /*16*/ virtual void __unk_vfn_2();
    /*17*/ virtual std::string const& getCustomName() const;
    /*18*/ virtual std::string getName() const;
    /*19*/ virtual std::string getImmersiveReaderText(class BlockSource&);
    /*20*/ virtual int getRepairCost() const;
    /*21*/ virtual class PistonBlockActor* getOwningPiston(class BlockSource&);
    /*22*/ virtual void __unk_vfn_3();
    /*23*/ virtual void __unk_vfn_4();
    /*24*/ virtual float getDeletionDelayTimeSeconds() const;
    /*25*/ virtual void __unk_vfn_5();
    /*26*/ virtual void __unk_vfn_6();
    /*27*/ virtual void __unk_vfn_7();
    /*28*/ virtual std::unique_ptr<class BlockActorDataPacket> _getUpdatePacket(class BlockSource&);
    /*29*/ virtual void _onUpdatePacket(class CompoundTag const&, class BlockSource&);
    /*30*/ virtual bool _playerCanUpdate(class Player const&) const;
    MCAPI EndGatewayBlockActor(class BlockPos const&);
    MCAPI void setExitPosition(class BlockPos const&);
    MCAPI void teleportEntity(class Actor&);
    MCAPI static int const COOLDOWN_TIME;
    MCAPI static int const EVENT_COOLDOWN;
    MCAPI static int const SPAWN_TIME;
    MCAPI static class BlockPos findExitPortal(class WorldGenerator&, class BlockPos const&);
    MCAPI static class BlockPos findValidSpawnAround(class BlockSource&, class BlockPos const&, bool, int);

protected:

private:
    MCAPI static int _getHighestSection(class WorldGenerator&, class BlockVolume&, class BlockPos const&);

};