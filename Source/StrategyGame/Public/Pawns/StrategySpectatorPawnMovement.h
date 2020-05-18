// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategySpectatorPawnMovement.generated.h"

UCLASS()
class UStrategySpectatorPawnMovement : public USpectatorPawnMovement
{
	GENERATED_UCLASS_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bInitialLocationSet;
};

