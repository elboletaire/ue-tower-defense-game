// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyBuilding.h"
#include "StrategyBuilding_Brewery.generated.h"

class AStrategyChar;
class UStrategyAIDirector;
class AStrategyBuilding;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaveSpawnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameFinishedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConstructedUpgradeDelegate, AStrategyBuilding*, ConstructedBuilding);


UCLASS(Abstract, Blueprintable)
class AStrategyBuilding_Brewery : public AStrategyBuilding
{
	GENERATED_UCLASS_BODY()

private:
	/** team's AI director */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Brewery, meta = (AllowPrivateAccess = "true"))
	UStrategyAIDirector* AIDirector;
public:

	/** The class of minion to spawn. */
	UPROPERTY(EditDefaultsOnly, Category=Brewery)
	TSubclassOf<AStrategyChar> MinionCharClass;

	/** left slot for upgrades to place in */
	UPROPERTY(EditInstanceOnly, Category=Brewery)
	TWeakObjectPtr<AStrategyBuilding>	LeftSlot;

	/** right slot for upgrades to place in */
	UPROPERTY(EditInstanceOnly, Category=Brewery)
	TWeakObjectPtr<AStrategyBuilding>	RightSlot;

	/** cost of each requested spawn */
	UPROPERTY(EditDefaultsOnly, Category=GameConfig)
	int32	SpawnCost;

	/** initial resources on easy difficulty */
	UPROPERTY(EditDefaultsOnly, Category=GameConfig)
	int32 ResourceInitialEasy;

	/** initial resources on medium difficulty */
	UPROPERTY(EditDefaultsOnly, Category=GameConfig)
	int32 ResourceInitialMedium;

	/** initial resources on hard difficulty */
	UPROPERTY(EditDefaultsOnly, Category=GameConfig)
	int32 ResourceInitialHard;

	/** delegate to broadcast about finished wave */
	UPROPERTY(BlueprintAssignable)
	FWaveSpawnedDelegate OnWaveSpawned;

	/** Event delegate for building upgrade construction complete. */
	UPROPERTY(BlueprintAssignable)
	FConstructedUpgradeDelegate OnConstructedUpgrade;

	/** class for empty slot */
	UPROPERTY()
	TSubclassOf<AStrategyBuilding> EmptySlotClass;

	/** get current number of lives */
	UFUNCTION(BlueprintCallable, Category=Brewery)
	uint8 GetNumberOfLives() const;

	/** set current number of lives*/
	UFUNCTION(BlueprintCallable, Category=Brewery)
	void SetNumberOfLives(uint8 NumberOfLives);

	/** event about constructed upgrade */
	virtual void OnConstructedBuilding(AStrategyBuilding* ConstructedUpgrade);

	// Begin Actor interface
	/** initial setup */
	virtual void PostInitializeComponents() override;
	// End Actor interface


	// Begin StrategyBuilding interface

	/** change current team */
	virtual void SetTeamNum(uint8 NewTeamNum) override;

	/** replace building with other class. return true if this building should never be built again */
	virtual bool ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass) override;

	/** add additional button for spawning dwarfs here*/
	virtual void ShowActionMenu() override;

	// End StrategyBuilding interface

	/** spawns a dwarf */
	bool SpawnDwarf();

	/** gets spawn queue length string */
	FText GetSpawnQueueLength() const;

	/** notify about new game state */
	void OnGameplayStateChange(EGameplayState::Type NewState);

protected:
	/** Number of lives. */
	uint8	NumberOfLives;

public:
	/** Returns AIDirector subobject **/
	FORCEINLINE UStrategyAIDirector* GetAIDirector() const { return AIDirector; }
};
