// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategySelectionInterface.generated.h"

/** Interface for actors which receive notifies about their selected state */
UINTERFACE()
class UStrategySelectionInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IStrategySelectionInterface
{
	GENERATED_IINTERFACE_BODY()

	/** tries to select actor */
	UFUNCTION(BlueprintNativeEvent)
	bool OnSelectionGained();

	/** tries to deselect actor */
	UFUNCTION(BlueprintNativeEvent)
	bool OnSelectionLost(const FVector& NewPosition, AActor* NewActor);
};
