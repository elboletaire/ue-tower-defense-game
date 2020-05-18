// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyCheatManager.generated.h"

UCLASS(Within=StrategyPlayerController)
class UStrategyCheatManager : public UCheatManager
{
	GENERATED_UCLASS_BODY()

	/** 
	 * Give the player gold.
	 *
	 * @param NewGold	The amount of gold to add to the players gold. 
	 */
	UFUNCTION(exec)
	void AddGold(uint32 NewGold);
};
