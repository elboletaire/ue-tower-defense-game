// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyMenuGameMode.h"
#include "StrategyMenuPlayerController.h"
#include "StrategyMenuHUD.h"
#include "StrategySpectatorPawn.h"

AStrategyMenuGameMode::AStrategyMenuGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//setup our custom PC and HUD
	PlayerControllerClass = AStrategyMenuPlayerController::StaticClass();
	HUDClass = AStrategyMenuHUD::StaticClass();
	SpectatorClass = AStrategySpectatorPawn::StaticClass();
}

APawn* AStrategyMenuGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot)
{
	return NULL;
}

