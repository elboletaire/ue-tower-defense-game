// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "StrategyTypes.h"
#include "StrategyAIAction.h"
#include "StrategyAIController.h"
#include "StrategyAIAction_MoveToBrewery.generated.h"


class INavigationPathGenerator;

UCLASS()
class UStrategyAIAction_MoveToBrewery : public UStrategyAIAction
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
	/** Called from owning controller when given PathGenerator updated it's path. */
	void OnPathUpdated(INavigationPathGenerator* PathGenerator, EPathUpdate::Type inType);

	/** notify about completing current move */
	void OnMoveCompleted();

	/** Acceptable distance to target destination */
	float TargetAcceptanceRadius;

	/** current destination we are moving to */
	FVector	Destination;

	/** tells if we stared moving to target */
	uint8	bIsMoving : 1;

	/** last time without movement */
	float	NotMovingFromTime;
};
