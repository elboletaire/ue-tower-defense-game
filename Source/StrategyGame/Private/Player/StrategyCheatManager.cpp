// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyCheatManager.h"


UStrategyCheatManager::UStrategyCheatManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UStrategyCheatManager::AddGold(uint32 NewGold)
{
	AStrategyGameState* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	FPlayerData* const TeamData = (MyGameState) ? MyGameState->GetPlayerData(EStrategyTeam::Player) : NULL;

	if (TeamData)
	{
		TeamData->ResourcesAvailable += NewGold;
		TeamData->ResourcesGathered += NewGold;

		AStrategyPlayerController* MyPC = Cast<AStrategyPlayerController>(GetOuter());
		if (MyPC)
		{
			FString Str = FString::Printf(TEXT("Gold added: %d"), NewGold);
			MyPC->ClientMessage(Str);
		}
	}
}
