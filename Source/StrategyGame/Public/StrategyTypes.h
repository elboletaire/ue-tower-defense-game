// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "StrategyTypes.generated.h"

#pragma once

UENUM(BlueprintType)
namespace EStrategyTeam
{
	enum Type
	{
		Unknown,
		Player,
		Enemy,
		MAX
	};
}

namespace EGameKey
{
	enum Type
	{
		Tap,
		Hold,
		Swipe,
		SwipeTwoPoints,
		Pinch,
	};
}

namespace EGameplayState
{
	enum Type
	{
		Waiting,
		Playing,
		Finished,
	};
}

UENUM()
namespace EGameDifficulty
{
	enum Type
	{
		Easy,
		Medium,
		Hard,
	};
}

DECLARE_DELEGATE_RetVal(bool, FActionButtonDelegate);
DECLARE_DELEGATE_RetVal(FText, FGetQueueLength)

struct FActionButtonData
{
	FText StrButtonText;
	FText StrTooltip;
	EVisibility Visibility;
	bool bIsEnabled;
	int32 ActionCost;
	UTexture2D*	ActionIcon;
	FActionButtonDelegate TriggerDelegate;
	FGetQueueLength GetQueueLengthDelegate;

	FActionButtonData()
	{
		StrButtonText = FText::GetEmpty();
		StrTooltip = FText::GetEmpty();
		bIsEnabled = true;
		ActionCost = 0;
		Visibility = EVisibility::Hidden;
		ActionIcon = NULL;
	}
};

struct FActionButtonInfo
{
	TSharedPtr<class SStrategyButtonWidget> Widget;
	FActionButtonData Data;
};

USTRUCT()
struct FPawnData
{
	GENERATED_USTRUCT_BODY()

	/** minimal attack damage */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackMin;

	/** maximum attack damage */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackMax;

	/** range of pawn attack */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 AttackDistance;

	/** damage reduction */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 DamageReduction;

	/** maximum health increase */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 MaxHealthBonus;

	/** health change every 5 seconds */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	int32 HealthRegen;

	/** movement speed */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	float Speed;

	/** defaults */
	FPawnData()
	{
		AttackMin = 5;
		AttackMax = 10;
		DamageReduction = 0;
		MaxHealthBonus = 0;
		HealthRegen = 10;
		Speed = 0.0;
		AttackDistance = 100;
	}
};

USTRUCT()
struct FBuffData
{
	GENERATED_USTRUCT_BODY()

	/** additive values */
	UPROPERTY(EditDefaultsOnly, Category=Data)
	FPawnData BuffData;

	/** set to ignore buff duration, not time limited */
	UPROPERTY(EditDefaultsOnly, Category=Buff)
	uint8 bInfiniteDuration : 1;

	/** buff duration in seconds */
	UPROPERTY(EditDefaultsOnly, Category=Buff)
	float Duration;

	/** runtime: buff ending time calculated when it's added */
	float EndTime;

	/** defaults */
	FBuffData()
	{
		bInfiniteDuration = false;
		Duration = 20.0f;
		EndTime = 0.0f;
	}

	/** 
	* Helper function for applying buff data.
	*
	* @param	PawnData		Data to apply.
	*/
	void ApplyBuff(struct FPawnData& PawnData);
};

struct FPlayerData
{
	/** current resources */
	uint32 ResourcesAvailable;

	/** total resources gathered */
	uint32 ResourcesGathered;

	/** total damage done */
	uint32 DamageDone;

	/** HQ */
	TWeakObjectPtr<class AStrategyBuilding_Brewery> Brewery;

	/** player owned buildings list */
	TArray<TWeakObjectPtr<class AActor>> BuildingsList;
};
