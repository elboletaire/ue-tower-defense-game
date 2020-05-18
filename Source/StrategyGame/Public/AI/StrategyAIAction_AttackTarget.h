// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyAIAction.h"
#include "StrategyAIAction_AttackTarget.generated.h"

UCLASS()
class UStrategyAIAction_AttackTarget : public UStrategyAIAction
{
	GENERATED_UCLASS_BODY()

public:

	// Begin StrategyAIAction interface

	/** Update in time current action */
	virtual bool Tick(float DeltaTime) override;

	/** activate action */
	virtual void Activate() override;

	/** abort action to start something else */
	virtual void Abort() override;

	/** can we abort this action? */
	virtual bool IsSafeToAbort() const override;

	/** Should we activate action this time ? */
	virtual bool ShouldActivate() const override;

	// End StrategyAIAction interface

protected:
	/** Pawn has hit something */
	void NotifyBump(FHitResult const& Hit);

	/** notify about completing current move */
	void OnMoveCompleted();

	/** move closer to target */
	void MoveCloser();

	/** updates any information about target, his location, target changes in ai controller, etc. */
	void UpdateTargetInformation();

	/** target actor to attack */
	TWeakObjectPtr<AActor> TargetActor;

	/** destination to move closer */
	FVector	TargetDestination;

	/** time when we will finish playing melee animation */
	float MeleeAttackAnimationEndTime;

	/** if pawn is playing attack animation */
	uint32 bIsPlayingAnimation : 1;

	/** set to true when we are moving to our target */
	uint32 bMovingToTarget : 1;
};
