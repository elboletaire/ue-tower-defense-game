// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyInputInterface.generated.h"

/** Interface for actors which can be selected */
UINTERFACE()
class UStrategyInputInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IStrategyInputInterface
{
	GENERATED_IINTERFACE_BODY()

	/** receive input: tap */
	UFUNCTION(BlueprintNativeEvent)
	void OnInputTap();

	/** receive input: hold */
	UFUNCTION(BlueprintNativeEvent)
	void OnInputHold();

	/** receive input: hold released */
	UFUNCTION(BlueprintNativeEvent)
	void OnInputHoldReleased(float DownTime);

	/** receive input: swipe update (world space, not screen space)*/
	UFUNCTION(BlueprintNativeEvent)
	void OnInputSwipeUpdate(const FVector& DeltaPosition);

	/** receive input: swipe finished (world space, not screen space) */
	UFUNCTION(BlueprintNativeEvent)
	void OnInputSwipeReleased(const FVector& DeltaPosition, float DownTime);
};
