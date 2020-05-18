// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyMenuPlayerController.h"
#include "Camera/PlayerCameraManager.h"

AStrategyMenuPlayerController::AStrategyMenuPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// just use the normal camera...fine for now
	PlayerCameraManagerClass = APlayerCameraManager::StaticClass();
}

void AStrategyMenuPlayerController::SetupInputComponent()
{
	// Skip AStrategyPlayerController::SetupInputComponent
	APlayerController::SetupInputComponent();
}