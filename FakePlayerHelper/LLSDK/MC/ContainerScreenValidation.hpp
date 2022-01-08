// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ContainerScreenValidation {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_CONTAINERSCREENVALIDATION
public:
    class ContainerScreenValidation& operator=(class ContainerScreenValidation const&) = delete;
    ContainerScreenValidation(class ContainerScreenValidation const&) = delete;
    ContainerScreenValidation() = delete;
#endif

public:
    /*0*/ virtual ~ContainerScreenValidation();
    /*1*/ virtual struct ContainerValidationResult tryTransfer(struct ContainerValidationSlotData const&, struct ContainerValidationSlotData const&, int, bool);
    /*2*/ virtual class std::shared_ptr<class SimpleSparseContainer> getOrCreateSparseContainer(enum ContainerEnumName);
    /*3*/ virtual enum ItemAddType _canAdd(struct ContainerValidatorPairScope const&, int, class ItemStackBase const&, int) const;
    /*4*/ virtual enum ItemSetType _canSet(struct ContainerValidatorPairScope const&, int, class ItemStackBase const&, int) const;
    /*5*/ virtual bool _canRemove(struct ContainerValidatorPairScope const&, int, int, bool) const;
    /*6*/ virtual bool _canDestroy(struct ContainerValidatorPairScope const&) const;
    MCAPI ContainerScreenValidation(class ContainerScreenContext const&, enum ContainerValidationCaller);
    MCAPI bool tryCommitActionResults();
    MCAPI struct ContainerValidationResult tryConsume(struct ContainerValidationSlotData const&, int);
    MCAPI struct ContainerValidationResult tryDestroy(struct ContainerValidationSlotData const&, int);
    MCAPI struct ContainerValidationResult tryDrop(struct ContainerValidationSlotData const&, int, bool);

protected:
    MCAPI class ItemStack _tryRemoveItem(struct ContainerValidatorPairScope&, int, int, bool);
    MCAPI struct ContainerValidationResult _tryTransferSpecial(struct ContainerValidationSlotData const&, int, enum ContainerScreenRequestActionType);

private:
    MCAPI struct ContainerValidatorPairScope _createContainerValidatorPairScope(enum ContainerEnumName);
    MCAPI class std::shared_ptr<class ContainerValidationBase const> _getOrCreateContainerValidator(enum ContainerEnumName);
    MCAPI bool _isValidSlot(struct ContainerValidatorPairScope const&, int) const;

};