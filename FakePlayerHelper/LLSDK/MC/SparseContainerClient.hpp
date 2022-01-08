// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class SparseContainerClient {

#define AFTER_EXTRA
// Add Member There
public:
enum PushSlotPredictionResult;

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SPARSECONTAINERCLIENT
public:
    class SparseContainerClient& operator=(class SparseContainerClient const&) = delete;
    SparseContainerClient(class SparseContainerClient const&) = delete;
    SparseContainerClient() = delete;
#endif

public:
    MCAPI void _networkUpdateItem(int, class ItemStack const&);

protected:

private:
    MCAPI static class BidirectionalUnorderedMap<enum SparseContainerClient::PushSlotPredictionResult, std::string > const pushSlotPredictionResultMap;

};