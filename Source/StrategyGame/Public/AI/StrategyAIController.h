// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AIController.h"
#include "StrategyTeamInterface.h"
#include "StrategyAIController.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogStrategyAI, Display, All);

namespace EPathUpdate
{
	enum Type
	{
		Update,
		Invalid,
		Failed
	};
}

DECLARE_DELEGATE_OneParam(FOnBumpEvent, FHitResult const&);
DECLARE_DELEGATE(FOnMovementEvent);

class APawn;
class AActor;
class UStrategyAIAction;
class UStrategyAISensingComponent;

UCLASS()
class AStrategyAIController : public AAIController, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

private:
	/** Component used to detect and respond to sight and sound events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta = (AllowPrivateAccess = "true"))
	class UStrategyAISensingComponent* SensingComponent;
public:

	/** List of all possible actions for this logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIController)
	TArray<TSubclassOf<UStrategyAIAction> > AllowedActions;

	/** List for all actions for this logic instance */
	UPROPERTY()
	TArray<UStrategyAIAction*> AllActions;

	/** Current, selected action to execute */
	UPROPERTY()
	UStrategyAIAction* CurrentAction;

	/** List of all possible targets for us */
	UPROPERTY()
	TArray<AActor*>	AllTargets;

	/** Current selected target to attack */
	UPROPERTY()
	AActor*			CurrentTarget;

public:
	// Begin AActor Interface
	virtual void Tick(float DeltaTime) override;

#if ENABLE_VISUAL_LOG
	/** Display additional data in visual logger */
	virtual void GrabDebugSnapshot(struct FVisualLogEntry* Snapshot) const override;
#endif
	// End AActor Interface

protected:
	// Begin Controller Interface
	virtual void OnPossess(APawn* inPawn) override;
	virtual void OnUnPossess() override;
	// End Controller Interface

public:
	// Begin AIController Interface
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	// End AIController Interface

	// Begin StrategyTeamInterface Interface
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface Interface

	/** Checks if we are allowed to use some action */
	bool IsActionAllowed(TSubclassOf<UStrategyAIAction> inClass) const;

	/** Return instance of action we are allowed to use */
	UStrategyAIAction* GetInstanceOfAction(TSubclassOf<UStrategyAIAction> inClass) const;

	/** pawn has hit something */
	void NotifyBump(FHitResult const& Hit);

	/** get data for current team */
	struct FPlayerData* GetTeamData() const;

	/** master switch: allows disabling all interactions */
	void EnableLogic(bool bEnable);

	/** returns information if we have logic enabled or disabled */
	bool IsLogicEnabled() const;

	/** Checks actor and returns true if valid */
	bool IsTargetValid(AActor* InActor) const;

	/** Claim controller as target */
	void ClaimAsTarget(TWeakObjectPtr<AStrategyAIController> InController);

	/** UnClaim controller as target */
	void UnClaimAsTarget(TWeakObjectPtr<AStrategyAIController> InController);

	/** Check if desired controller claimed this one */
	bool IsClaimedBy(TWeakObjectPtr<AStrategyAIController> InController) const;

	/** get number of enemies who claimed this one as target */
	int32 GetNumberOfAttackers() const;

	/** register movement related notify, to get notify about completed movement */
	void RegisterMovementEventDelegate(FOnMovementEvent);
	/** unregister movement related notify*/
	void UnregisterMovementEventDelegate();

	/** register bump notify, to get notify when pawn bumps with something */
	void RegisterBumpEventDelegate(FOnBumpEvent);
	/** unregister bump notify */
	void UnregisterBumpEventDelegate();

	/** @return If this is a pawn return its location or the actor location */
	virtual FVector GetAdjustLocation();


protected:
	/** Check targets list and select one as current target */
	virtual void SelectTarget();

protected:
	/** array of controllers claimed this one as target */
	TArray< TWeakObjectPtr<AStrategyAIController> > ClaimedBy;

	/** Event delegate for when pawn movement is complete. */
	FOnMovementEvent OnMoveCompletedDelegate;

	/** Event delegate for when pawn has hit something. */
	FOnBumpEvent OnNotifyBumpDelegate;

	/** master switch state */
	uint8 bLogicEnabled : 1;

public:
	/** Returns SensingComponent subobject **/
	FORCEINLINE UStrategyAISensingComponent* GetSensingComponent() const { return SensingComponent; }
};
