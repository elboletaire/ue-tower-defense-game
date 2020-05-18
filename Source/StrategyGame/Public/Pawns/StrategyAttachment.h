// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyAttachment.generated.h"


UCLASS(Blueprintable)
class UStrategyAttachment : public USkeletalMeshComponent
{
	GENERATED_UCLASS_BODY()

	/** BuffData representing attachable effects on the pawn */
	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	FBuffData Effect;

	/** Attach point on pawn */
	UPROPERTY(EditDefaultsOnly, Category=Attachment)
	FName AttachPoint;
};
