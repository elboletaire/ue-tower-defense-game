// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyInputInterface.h"
#include "StrategyResourceNode.generated.h"


UCLASS(Blueprintable)
class AStrategyResourceNode : public AActor, public IStrategyInputInterface
{
	GENERATED_UCLASS_BODY()

	/** reset resource actor to reuse it again in game (it'll not unhide this actor) */
	UFUNCTION(BlueprintCallable, Category=ResourceNode)
	void ResetResource(bool UnhideInGame=true);

public:
	//////////////////////////////////////////////////////////////////////////
	// input

	// Begin StrategyInputInterface interface

	/** [IStrategyInputInterface] receive input: tap */
	virtual void OnInputTap_Implementation() override;

	/** [IStrategyInputInterface] receive input: hold */
	virtual void OnInputHold_Implementation() override {}

	/** [IStrategyInputInterface] receive input: hold released */
	virtual void OnInputHoldReleased_Implementation(float DownTime) override {}

	/** [IStrategyInputInterface] receive input: swipe update */
	virtual void OnInputSwipeUpdate_Implementation(const FVector& DeltaPosition) override {}

	/** [IStrategyInputInterface] receive input: swipe finished */
	virtual void OnInputSwipeReleased_Implementation(const FVector& DeltaPosition, float DownTime) override {}

	// End StrategyInputInterface interface

	//////////////////////////////////////////////////////////////////////////
	// reading data
	
	/** amount of resources left in node */
	int32 GetAvailableResources() const;

	/** initial amount of resources */
	int32 GetInitialResources() const;

protected:

	/** resources in node */
	UPROPERTY(EditDefaultsOnly, Category=ResourceNode)
	int32 NumResources;

	/** blueprint event: demolished */
	UFUNCTION(BlueprintImplementableEvent, Category=ResourceNode)
	void OnDepleted();
};
