// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTeamInterface.generated.h"

/** Interface for actors which can be associated with teams */
UINTERFACE()
class UStrategyTeamInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IStrategyTeamInterface
{
	GENERATED_IINTERFACE_BODY()

	/** returns the team number of the actor */
	virtual uint8 GetTeamNum() const = 0;
};
