// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Packet.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here
enum PlayerActionType {
    START_BREAK,
    ABORT_BREAK,
    STOP_BREAK,
    GET_UPDATED_BLOCK,
    DROP_ITEM,
    START_SLEEP,
    STOP_SLEEP,
    RESPAWN,
    JUMP,
    START_SPRINT,
    STOP_SPRINT,
    START_SNEAK,
    STOP_SNEAK,
    DIMENSION_CHANGE_REQUEST_OR_CREATIVE_DESTROY_BLOCK,
    DIMENSION_CHANGE_SUCCESS,
    START_GLIDE,
    STOP_GLIDE,
    BUILD_DENIED,
    CONTINUE_BREAK,
    CHANGE_SKIN,
    SET_ENCHANTMENT_SEED,
    START_SWIMMING,
    STOP_SWIMMING,
    START_SPIN_ATTACK,
    STOP_SPIN_ATTACK,
    BLOCK_INTERACT,
    /**
     * @since v428
     */
     BLOCK_PREDICT_DESTROY,
     /**
      * @since v428
      */
      BLOCK_CONTINUE_DESTROY,
};

#undef BEFORE_EXTRA

class PlayerActionPacket : public Packet {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_PLAYERACTIONPACKET
public:
    class PlayerActionPacket& operator=(class PlayerActionPacket const&) = delete;
    PlayerActionPacket(class PlayerActionPacket const&) = delete;
#endif

public:
    /*0*/ virtual ~PlayerActionPacket();
    /*1*/ virtual enum MinecraftPacketIds getId() const;
    /*2*/ virtual std::string getName() const;
    /*3*/ virtual void write(class BinaryStream&) const;
    /*4*/ virtual bool disallowBatching() const;
    /*5*/ virtual enum StreamReadResult _read(class ReadOnlyBinaryStream&);
    /*
    inline  ~PlayerActionPacket(){
         (PlayerActionPacket::*rv)();
        *((void**)&rv) = dlsym("??1PlayerActionPacket@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI PlayerActionPacket(enum PlayerActionType, class BlockPos const&, unsigned char, class ActorRuntimeID);
    MCAPI PlayerActionPacket(enum PlayerActionType, class BlockPos const&, int, class ActorRuntimeID);
    MCAPI PlayerActionPacket(enum PlayerActionType, class ActorRuntimeID);
    MCAPI PlayerActionPacket();

protected:

private:

};