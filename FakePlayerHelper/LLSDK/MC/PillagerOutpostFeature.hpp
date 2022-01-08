// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "StructureFeature.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class PillagerOutpostFeature : public StructureFeature {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_PILLAGEROUTPOSTFEATURE
public:
    class PillagerOutpostFeature& operator=(class PillagerOutpostFeature const&) = delete;
    PillagerOutpostFeature(class PillagerOutpostFeature const&) = delete;
    PillagerOutpostFeature() = delete;
#endif

public:
    /*0*/ virtual ~PillagerOutpostFeature();
    /*1*/ virtual bool getNearestGeneratedFeature(class Dimension&, class BiomeSource const&, class BlockPos const&, class BlockPos&, class IPreliminarySurfaceProvider const&, bool);
    /*2*/ virtual bool isFeatureChunk(class BiomeSource const&, class Random&, class ChunkPos const&, unsigned int, class IPreliminarySurfaceProvider const&);
    /*3*/ virtual std::unique_ptr<class StructureStart> createStructureStart(class Dimension&, class BiomeSource const&, class Random&, class ChunkPos const&, class IPreliminarySurfaceProvider const&);
    /*
    inline  ~PillagerOutpostFeature(){
         (PillagerOutpostFeature::*rv)();
        *((void**)&rv) = dlsym("??1PillagerOutpostFeature@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI PillagerOutpostFeature(unsigned int);
    MCAPI static void initMobSpawnTypes(class HardcodedSpawnAreaRegistry&);

protected:

private:

};