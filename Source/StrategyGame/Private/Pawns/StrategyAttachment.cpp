// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAttachment.h"

UStrategyAttachment::UStrategyAttachment(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Effect.bInfiniteDuration = true;
}
