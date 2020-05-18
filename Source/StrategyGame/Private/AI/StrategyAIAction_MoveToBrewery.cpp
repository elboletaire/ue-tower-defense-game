// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAIAction_MoveToBrewery.h"
#include "StrategyAIController.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyAIDirector.h"
#include "NavigationPathGenerator.h"

#include "VisualLogger/VisualLogger.h"

UStrategyAIAction_MoveToBrewery::UStrategyAIAction_MoveToBrewery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetAcceptanceRadius(150)
	, Destination(FVector::ZeroVector)
	, bIsMoving(false)
	, NotMovingFromTime(0)
{
}

bool UStrategyAIAction_MoveToBrewery::IsSafeToAbort() const
{
	return true;
}

void UStrategyAIAction_MoveToBrewery::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	bIsMoving = false;
	Destination = FVector::ZeroVector;
	if (MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
	}
	MyAIController->UnregisterMovementEventDelegate();
}

void UStrategyAIAction_MoveToBrewery::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	NotMovingFromTime = 0;
	// find brewery base and cache it's destination
	const FPlayerData* TeamData = MyAIController->GetTeamData();
	if (TeamData != NULL && TeamData->Brewery != NULL && TeamData->Brewery->GetAIDirector() != NULL)
	{
		const AActor* Actor = TeamData->Brewery->GetAIDirector()->GetEnemyBrewery();
		if (Actor != NULL)
		{
			bIsMoving = true;
			Destination = Actor->GetActorLocation();
			MyAIController->MoveToLocation(Destination, TargetAcceptanceRadius, true, true, true);
		}
	}

	FOnMovementEvent MovementDelegate;
	MovementDelegate.BindUObject(this, &UStrategyAIAction_MoveToBrewery::OnMoveCompleted);
	MyAIController->RegisterMovementEventDelegate(MovementDelegate);
}

bool UStrategyAIAction_MoveToBrewery::Tick(float DeltaTime)
{
	if (bIsMoving && MyAIController.IsValid())
	{
		const bool bNoMove = (MyAIController->GetMoveStatus() != EPathFollowingStatus::Moving);
		if (!bNoMove)
		{
			NotMovingFromTime = 0;
		}
		else if (NotMovingFromTime == 0)
		{
			NotMovingFromTime = MyAIController->GetWorld()->TimeSeconds;
		}

		if (bNoMove && (MyAIController->GetWorld()->TimeSeconds - NotMovingFromTime) > 2)
		{
			Abort();
		}
	}
	return bIsExecuted && Destination != FVector::ZeroVector && bIsMoving;
}

bool UStrategyAIAction_MoveToBrewery::ShouldActivate() const
{
	check(MyAIController.IsValid());

	FVector DesiredDestination = FVector::ZeroVector;
	const FPlayerData* TeamData = MyAIController->GetTeamData();
	if (TeamData != NULL && TeamData->Brewery != NULL && TeamData->Brewery->GetAIDirector() != NULL)
	{
		const AActor* Actor = TeamData->Brewery->GetAIDirector()->GetEnemyBrewery();
		if (Actor != NULL)
		{
			DesiredDestination = Actor->GetActorLocation();
		}
	}

	if (DesiredDestination != FVector::ZeroVector)
	{
		return ((DesiredDestination - MyAIController->GetAdjustLocation()).Size2D() > TargetAcceptanceRadius);
	}
	return false;
}

void UStrategyAIAction_MoveToBrewery::OnMoveCompleted()
{
	bIsMoving = false;
}

void UStrategyAIAction_MoveToBrewery::OnPathUpdated(INavigationPathGenerator* PathGenerator, EPathUpdate::Type inType)
{
	check(MyAIController.IsValid());

	if (inType != EPathUpdate::Update)
	{
		UE_VLOG(MyAIController.Get(), LogStrategyAI, Log, TEXT("WARRNING, OnPathUpdated with error - PathUpdateTyp %d"), int32(inType)); 
		Abort();
	}
}
