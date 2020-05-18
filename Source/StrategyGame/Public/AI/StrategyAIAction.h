// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once


#include "StrategyAIAction.generated.h"

class AStrategyAIController;

UCLASS(abstract, BlueprintType)
class UStrategyAIAction : public UObject
{
	GENERATED_UCLASS_BODY()

	/** 
	 * Update this action.
	 * @return false to finish this action, true to continue
	 */
	virtual bool Tick(float DeltaTime);

	/** Activate action. */
	virtual void Activate();

	/** Should we activate action this time ? */
	virtual bool ShouldActivate() const;

	/** Abort action to start something else. */
	virtual void Abort();

	/** Can we abort this action? */
	virtual bool IsSafeToAbort() const;

	/** Set owning AI controller. */
	void SetController(AStrategyAIController* InOwner);

protected:
	/** Weak pointer to AI controller, to have faster access (cached information). */
	TWeakObjectPtr<AStrategyAIController>	MyAIController;

	/**	Tells us if we are already executed. */
	bool bIsExecuted;
};
