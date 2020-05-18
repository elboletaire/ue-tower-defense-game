// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAISensingComponent.h"

UStrategyAISensingComponent::UStrategyAISensingComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	SensingInterval = 0.2f;
	SightRadius = 2200.0f;
	SetPeripheralVisionAngle(180.0f);
	bOnlySensePlayers = false;
	bHearNoises = false;
	bSeePawns = true;
}

void UStrategyAISensingComponent::InitializeComponent()
{
	Super::InitializeComponent();
	// set custom data from config file
	SightRadius = SightDistance;
}

bool UStrategyAISensingComponent::ShouldCheckVisibilityOf(APawn *Pawn) const
{
	AStrategyChar* const TestChar = Cast<AStrategyChar>(Pawn);
	return TestChar != nullptr && !TestChar->bHidden && TestChar->GetHealth() > 0 && AStrategyGameMode::OnEnemyTeam(Pawn, GetOwner());
}

bool UStrategyAISensingComponent::CanSenseAnything() const
{
	return SightRadius > 0.0f;
}

void UStrategyAISensingComponent::UpdateAISensing()
{
	const AActor* const Owner = GetOwner();
	if (!IsValid(Owner) || (Owner->GetWorld() == NULL))
	{
		// Cannot sense without a valid owner in the world.
		return;
	}

	for (AStrategyChar* const TestChar : TActorRange<AStrategyChar>(Owner->GetWorld()))
	{
		if (!IsSensorActor(TestChar) && ShouldCheckVisibilityOf(TestChar))
		{
			if (CouldSeePawn(TestChar, true))
			{
				KnownTargets.AddUnique(TestChar);
			}
		}
	}

	for (int32 i = KnownTargets.Num() - 1; i >= 0; i--)
	{
		const AStrategyChar* TestChar = Cast<AStrategyChar>(KnownTargets[i].Get());
		if (TestChar == NULL)
		{
			KnownTargets.RemoveAt(i);
		}
	}
}
