#pragma once
#include <string>
#include <memory>
#include <mc\Dimension.h>
#include <mc\Actor.h>

enum class OriginType : char;

class CommandOrigin {
public:
	CommandOrigin(CommandOrigin const&) = delete;
	virtual					~CommandOrigin() = 0;
	virtual std::string&	getRequestId() const = 0;
	virtual std::string		getName() const = 0;
	virtual Vec3*			getBlockPosition(Vec3*) const = 0;
	virtual Vec3			getWorldPosition() const = 0;
	virtual __int64			getLevel() const = 0;// Level* getLevel() const
	virtual Dimension*		getDimension() const = 0;
	virtual Actor*			getEntity() const = 0;
	virtual __int64			getPermissionsLevel() const = 0;// enum CommandPermissionLevel getPermissionsLevel() const
	virtual std::unique_ptr<CommandOrigin> clone() const = 0;
	virtual bool			unk1() const = 0;
	virtual bool			unk2() const = 0;
	virtual bool			hasChatPerms() const = 0;
	virtual bool			hasTellPerms() const = 0;
	virtual bool			canUseAbility(__int64) const = 0;// bool canUseAbility(AbilitiesIndex) const
	virtual bool			isWorldBuilder() const = 0;
	virtual bool			canUseCommandsWithoutCheatsEnabled() const = 0;
	virtual bool			isSelectorExpansionAllowed() const = 0;
	virtual __int64			getSourceId_net() const = 0;// NetworkIdentifier getSourceId_net() const
	virtual char			getSourceSubId() const = 0;
	virtual CommandOrigin*	getOutputReceiver() const = 0;
	virtual OriginType		getOriginType() const = 0;
	virtual void			toCommandOriginData_stub() const = 0;
	virtual __int64 const&	getUUID() const = 0;// mce::UUID const& getUUID() const
	virtual void			handleCommandOutputCallback_stub(void*) = 0;
	virtual void			_setUUID(__int64 const&) = 0;// void _setUUID(mce::UUID const&)
};

