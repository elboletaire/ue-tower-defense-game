// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyResourceNode.h"


AStrategyResourceNode::AStrategyResourceNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NumResources(100)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AStrategyResourceNode::OnInputTap_Implementation()
{
	if ( !(AActor::IsHidden()) && (NumResources > 0) )
	{
		AStrategyGameState* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
		FPlayerData* const TeamData = MyGameState ? MyGameState->GetPlayerData(EStrategyTeam::Player) : NULL;

		if (TeamData)
		{
			TeamData->ResourcesAvailable += NumResources;
			TeamData->ResourcesGathered += NumResources;
		}

		NumResources = 0;
		OnDepleted();
	}
}

void AStrategyResourceNode::ResetResource(bool UnhideInGame)
{
	NumResources = GetClass()->GetDefaultObject<AStrategyResourceNode>()->NumResources;
	if (UnhideInGame)
	{
		SetActorHiddenInGame(false);
	}
}


int32 AStrategyResourceNode::GetAvailableResources() const
{
	return NumResources;
}

int32 AStrategyResourceNode::GetInitialResources() const
{
	return GetClass()->GetDefaultObject<AStrategyResourceNode>()->GetAvailableResources();
}
