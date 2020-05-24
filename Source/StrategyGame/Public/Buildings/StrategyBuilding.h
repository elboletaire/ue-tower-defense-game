// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyInputInterface.h"
#include "StrategyTeamInterface.h"
#include "StrategySelectionInterface.h"
#include "StrategyBuilding.generated.h"


DECLARE_DELEGATE_OneParam(FBuildFinishedDelegate, class AStrategyBuilding*);

class AStrategyChar;

UCLASS(Abstract, Blueprintable)
class AStrategyBuilding : public AActor,
	public IStrategyInputInterface, public IStrategyTeamInterface, public IStrategySelectionInterface
{
	GENERATED_UCLASS_BODY()

	/** team */
	UPROPERTY(EditInstanceOnly, Category=Building)
	TEnumAsByte<EStrategyTeam::Type> SpawnTeamNum;

	/** multicast about finished construction */
	FBuildFinishedDelegate BuildFinishedDelegate;

	// Begin Actor interface
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLoad() override;
	// End Actor Interface

	//////////////////////////////////////////////////////////////////////////
	// Begin StrategySelectionInterface interface

	/** [IStrategySelectionInterface] tries to select actor */
	virtual bool OnSelectionGained_Implementation() override;

	/** [IStrategySelectionInterface] tries to deselect actor */
	virtual bool OnSelectionLost_Implementation(const FVector& NewPosition, AActor* NewActor) override;
	// End StrategySelectionInterface interface

	//////////////////////////////////////////////////////////////////////////
	// Begin StrategyInputInterface interface

	/** [IStrategyInputInterface] receive input: tap */
	virtual void OnInputTap_Implementation() override;

	/** [IStrategyInputInterface] receive input: hold */
	virtual void OnInputHold_Implementation() override;

	/** [IStrategyInputInterface] receive input: hold released */
	virtual void OnInputHoldReleased_Implementation(float DownTime) override;

	/** [IStrategyInputInterface] receive input: swipe update */
	virtual void OnInputSwipeUpdate_Implementation(const FVector& DeltaPosition) override;

	/** [IStrategyInputInterface] receive input: swipe finished */
	virtual void OnInputSwipeReleased_Implementation(const FVector& DeltaPosition, float DownTime) override;

	// End StrategyInputInterface interface

	//////////////////////////////////////////////////////////////////////////
	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	virtual uint8 GetTeamNum() const override;

	// End StrategyTeamInterface interface

	/** set team number */
	virtual void SetTeamNum(uint8 NewTeamNum);

	//////////////////////////////////////////////////////////////////////////
	// Touch

	/** handle touch events */
	virtual void NotifyActorBeginOverlap(AActor* Other);

	/** Check if building can affect char */
	UFUNCTION(BlueprintCallable, Category=Building)
	bool CanAffectChar(const AStrategyChar* Char) const;

	//////////////////////////////////////////////////////////////////////////
	// Construction

	/** replace building with other class. return true if this building should never be built again */
	virtual bool ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass);

	/** replace building with other class, returns new building in second parameter. return true if this building should never be built again  */
	virtual bool ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass, AStrategyBuilding** OutNewBuilding);

	/** Switch building into build state */
	bool StartBuild();

	/** Build state is finished, switch building into normal state */
	void FinishBuild();

	/** Returns true if building process is finished, false otherwise. */
	bool IsBuildFinished();

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get current health */
	UFUNCTION(BlueprintCallable, Category=Health)
	int32 GetHealth() const;

	/** get max health */
	UFUNCTION(BlueprintCallable, Category=Health)
	int32 GetMaxHealth() const;

	/** get building's name */
	FString GetBuildingName() const;

	/** get building's cost */
	int32 GetBuildingCost(UWorld *World) const;

	/** get construction time */
	int32 GetBuildTime() const;

	/** return cost needed to finish building */
	float GetRemainingBuildCost() const;

protected:
	/** construction start sound stinger */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	USoundCue* ConstructionStartStinger;

	/** construction end sound stinger */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	USoundCue* ConstructionEndStinger;

	/** upgrade construction sound stinger */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	USoundCue* UpgradeStinger;

	/** cost of building */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	int32 Cost;

	/** cost of building */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	int32 AdditionalCost;

	/** construction time */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	int32 BuildTime;

	/** name of building */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	FString BuildingName;

	/** building icon for UI */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	UTexture2D* BuildingIcon;

	/** hit points for building */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	int32 Health;

private:
	/** trigger box component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Touch, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerBox;
protected:

	/** affect friendly minions? */
	UPROPERTY(EditDefaultsOnly, Category=Touch)
	uint8 bAffectFriendlyMinion : 1;

	/** affect enemy minions? */
	UPROPERTY(EditDefaultsOnly, Category=Touch)
	uint8 bAffectEnemyMinion : 1;

	/** if construction is finished, any build actions are repairs (cheaper) */
	UPROPERTY(EditInstanceOnly, Category=Building)
	uint8 bIsContructionFinished : 1;

	/** list of possible upgrades */
	UPROPERTY(EditDefaultsOnly, Category=Building)
	TArray<TSubclassOf<AStrategyBuilding> > Upgrades;

	/** is being build (inactive)? */
	uint8 bIsBeingBuild : 1;

	/** is action menu displayed? */
	uint8 bIsActionMenuDisplayed : 1;

	/** is custom action displayed? */
	uint8 bIsCustomActionDisplayed : 1;

	/** current team number */
	uint8 MyTeamNum;

	/** Built time if building is not attacked in the meantime */
	float InitialBuildTime;

	/** remaining build time */
	float RemainingBuildTime;

	/** get data for current team */
	struct FPlayerData* GetTeamData() const;

	//////////////////////////////////////////////////////////////////////////
	// UI

	/** display action menu for building */
	virtual void ShowActionMenu();

	/** hide action menu for building */
	UFUNCTION(BlueprintCallable, meta=(DisplayName = "Hide Action Menu"), Category=Building)
	virtual void HideActionMenu();

	/** collect possible upgrades */
	virtual void GetUpgradeList(TArray<TSubclassOf<AStrategyBuilding> >& Upgrades) const;

	//////////////////////////////////////////////////////////////////////////
	// blueprint events

	/** blueprint event: tap */
	UFUNCTION(BlueprintImplementableEvent, Category = Building)
	void OnTapEvent();

	/** blueprint event: hold */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnHoldEvent();

	/** blueprint event: hold released */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnHoldReleasedEvent(float DownTime);

	/** blueprint event: swipe update */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnSwipeUpdateEvent(const FVector& DeltaPosition);

	/** blueprint event: swipe released */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnSwipeReleasedEvent(const FVector& DeltaPosition, float DownTime);

	/** blueprint event: touch */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnCharTouch(AStrategyChar* Char);

	/** blueprint event: build started */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnBuildStarted();

	/** blueprint event: build finished */
	UFUNCTION(BlueprintImplementableEvent, Category=Building)
	void OnBuildFinished();

protected:
	/** Returns TriggerBox subobject **/
	FORCEINLINE UBoxComponent* GetTriggerBox() const { return TriggerBox; }
};
