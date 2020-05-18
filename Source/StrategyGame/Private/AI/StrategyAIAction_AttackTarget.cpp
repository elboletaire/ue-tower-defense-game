// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAIAction_AttackTarget.h"
#include "StrategyAIController.h"
#include "VisualLogger/VisualLogger.h"

UStrategyAIAction_AttackTarget::UStrategyAIAction_AttackTarget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, MeleeAttackAnimationEndTime(0)
, bIsPlayingAnimation(false)
, bMovingToTarget(false)
{
	// Non-property initialization
}

/** function to register as Update delegate for this action */
bool UStrategyAIAction_AttackTarget::Tick(float DeltaTime)
{
	check(MyAIController.IsValid());
	Super::Tick(DeltaTime);

	UpdateTargetInformation();

	bIsPlayingAnimation = MyAIController->GetWorld()->GetTimeSeconds() < MeleeAttackAnimationEndTime;
	if (!bIsPlayingAnimation)
	{
		if (!TargetActor.IsValid())
		{
			return false;
		}

		// try move closer if needed again
		MoveCloser();

		if (!bMovingToTarget)
		{
			AStrategyChar* const MyChar = Cast<AStrategyChar>(MyAIController->GetPawn());
			if (MyChar != NULL)
			{
				MeleeAttackAnimationEndTime = MyAIController->GetWorld()->GetTimeSeconds() + MyChar->PlayMeleeAnim();
				bIsPlayingAnimation = true;
			}
		}

		return MyAIController->IsTargetValid(TargetActor.Get());
	}

	return true;
}

void UStrategyAIAction_AttackTarget::UpdateTargetInformation()
{
	AActor* const OldTargetActor = TargetActor.Get();
	if (MyAIController->CurrentTarget != TargetActor.Get())
	{
		TargetActor = MyAIController->CurrentTarget;
	}

	if (OldTargetActor != TargetActor.Get() && bMovingToTarget)
	{
		bMovingToTarget = false;
	}

	if (MyAIController->IsTargetValid(TargetActor.Get()) )
	{
		MyAIController->SetFocus(TargetActor.Get());
	}
}

void UStrategyAIAction_AttackTarget::MoveCloser()
{
	check(MyAIController.IsValid());

	if (bIsPlayingAnimation || bMovingToTarget || TargetActor == nullptr)
	{
		return;
	}
		
	TargetDestination = TargetActor->GetActorLocation();
	AStrategyChar* const MyChar = Cast<AStrategyChar>(MyAIController->GetPawn());
	if( MyChar == nullptr )
	{
		UE_LOG(LogStrategyAI, Warning, TEXT("Invalid Char/Pawn in Move Closer")); 
		return;
	}
	
	check(MyChar->GetPawnData());

	const float AttackDistance = MyChar->GetPawnData()->AttackDistance;
	const float Dist = (TargetDestination - MyAIController->GetAdjustLocation()).Size2D();

	if (Dist > AttackDistance)
	{
		UE_VLOG(MyAIController.Get(), LogStrategyAI, Log, TEXT("Let's move closer")); 
		bMovingToTarget = true;
		MyAIController->MoveToActor(TargetActor.Get(), 0.9 * AttackDistance);
	}
}

void UStrategyAIAction_AttackTarget::OnMoveCompleted()
{
	bMovingToTarget = false;
	if (MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
	}
}

void UStrategyAIAction_AttackTarget::NotifyBump(FHitResult const& Hit)
{
	check(MyAIController.IsValid());

	// if we hit our target, just stop movement
	AStrategyChar* const HitChar = Cast<AStrategyChar>(Hit.Actor.Get());
	if (HitChar != NULL && AStrategyGameMode::OnEnemyTeam(HitChar, MyAIController->GetPawn()) && bMovingToTarget)
	{
		bMovingToTarget = false;
		if (MyAIController->GetPathFollowingComponent())
		{
			MyAIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
		}
	}
}

void UStrategyAIAction_AttackTarget::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	bIsPlayingAnimation = false;
	MeleeAttackAnimationEndTime = 0;
	TargetActor = MyAIController->CurrentTarget;

	FOnBumpEvent BumpDelegate;
	BumpDelegate.BindUObject(this, &UStrategyAIAction_AttackTarget::NotifyBump);
	MyAIController->RegisterBumpEventDelegate(BumpDelegate);

	FOnMovementEvent MovementDelegate;
	MovementDelegate.BindUObject(this, &UStrategyAIAction_AttackTarget::OnMoveCompleted);
	MyAIController->RegisterMovementEventDelegate(MovementDelegate);
}


bool UStrategyAIAction_AttackTarget::IsSafeToAbort() const
{
	return !bIsPlayingAnimation;
}

void UStrategyAIAction_AttackTarget::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	if (bMovingToTarget && MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
	}
	bMovingToTarget = false;
	MyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	MyAIController->UnregisterBumpEventDelegate();
	MyAIController->UnregisterMovementEventDelegate();
}

bool UStrategyAIAction_AttackTarget::ShouldActivate() const
{
	check(MyAIController.IsValid());
	return MyAIController->CurrentTarget != NULL && MyAIController->IsTargetValid(MyAIController->CurrentTarget);
}
