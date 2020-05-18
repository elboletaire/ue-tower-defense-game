// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyAIDirector.generated.h"

class AStrategyBuilding_Brewery;
class AStrategyChar;
class UStrategyAttachment;

UCLASS()
class UStrategyAIDirector : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	/** set default armor for spawns */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use SetDefaultArmorClass"))
	void SetDefaultArmor(UBlueprint* InArmor);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	void SetDefaultArmorClass(TSubclassOf<UStrategyAttachment> InArmor);

	/** set default wepon for spawns */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use new version"))
	void SetDefaultWeapon(UBlueprint* InWeapon);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	void SetDefaultWeaponClass(TSubclassOf<UStrategyAttachment> InWeapon);

	/** set default buff modifier for spawns */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DisplayName = "Set Unit Properties"))
	void SetBuffModifier(AStrategyChar* Pawn, int32 AttackMin, int32 AttackMax, int32 DamageReduction, int32 MaxHealthBonus, int32 HealthRegen, float Speed, int32 DrunkLevel, float Duration, bool bInfiniteDuration, float CustomScale = 1.0, float AnimaRate = 1);

	/** Number of pawns to spawn each wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Minions)
	int32 WaveSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Minions)
	float RadiusToSpawnOn;

protected:
	/** default armor for spawns */
	UPROPERTY()
	TSubclassOf<UStrategyAttachment> DefaultArmor;

	/** default weapon for spawns */
	UPROPERTY()
	TSubclassOf<UStrategyAttachment> DefaultWeapon;

	/** default modifier for spawns */
	UPROPERTY()
	FBuffData BuffModifier;

public:
	/** Override to return correct team number */
	virtual uint8 GetTeamNum() const;

	/** Override to return correct team number */
	virtual void SetTeamNum(uint8 inTeamNum);

	// Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);
	// End UActorComponent Interface

	/** Getter for brewery of enemy side */
	AStrategyBuilding_Brewery* GetEnemyBrewery() const;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

	/** request spawn from AI Director */
	void RequestSpawn();
protected:
	/** check conditions and spawn minions if possible */
	void SpawnMinions();

	/** Custom scale for spawns */
	float CustomScale;

	/** Custom animation rate for spawns */
	float AnimationRate;

	/** next time to spawn minion */
	float NextSpawnTime;

	/** team number */
	uint8 MyTeamNum;

	/** Brewery of my biggest enemy */
	TWeakObjectPtr<AStrategyBuilding_Brewery> EnemyBrewery;
};

