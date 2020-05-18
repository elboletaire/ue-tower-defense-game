// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameMode.generated.h"

class AController;
class AStrategyBuilding;
class SStrategyTitle;

UCLASS(config=Game)
class AStrategyGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

	/** Class for empty wall slot. */
	UPROPERTY()
	TSubclassOf<AStrategyBuilding> EmptyWallSlotClass;

	/** Time before game returns to menu after finish. */
	UPROPERTY(config)
	int32 TimeBeforeReturnToMenu;

	/** Name of the difficulty param on the URL options string. */
	static const FString DifficultyOptionName;
	
	// Begin GameMode interface

	/** Initialize the GameState actor. */
	virtual void InitGameState() override;

	/** 
	 * Handle new player, skips pawn spawning. 
	 * @param NewPlayer	
	 */
	virtual void RestartPlayer(AController* NewPlayer) override;
	
	/** 
	 * Modify the damage we want to apply to an actor.
	 * 
	  * @param Damage			The damage
	  * @param DamagedActor		The actor we wish to damage
	  * @param DamageEvent		The event that caused the damage
	  * @param EventInstigator	
	  * @param DamageCauser
	  *
	  * @returns The adjusted damage amount
	  */
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// End GameMode interface

	/** 
	 * Finish the game with selected team as winner.
	 *
	 * @param	InWinningTeam		The team that has won.
	 */
	UFUNCTION(BlueprintCallable, Category=Game)
	void FinishGame(EStrategyTeam::Type InWinningTeam);

	void ReturnToMenu();

	/** 
	 * Helper function to test teams (null = not in friendly team). 
	 *
	 * @param ActorA		First actor to test against
	 * @param ActorB		Second actor to test against
	 *
	 * @return true if the two actors are on the same team.
	 */
	static bool OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB);

	/** 
	 * Helper function to test teams (null = not in friendly team). 
	 *
	 * @param ActorA		First actor to test against
	 * @param ActorB		Second actor to test against
	 *
	 * @return true if the two actors are on opposing teams.
	 */	
	static bool OnEnemyTeam(const AActor* ActorA, const AActor* ActorB);

	/** Helper method for UI, to exit game. */
	void ExitGame();

	/** Pointer to title text widget. */
	TSharedPtr<SStrategyTitle> StrategyTitle;

protected:
	/* Helper to return the current gameplay state. */
	EGameplayState::Type GetGameplayState() const;

	/** Handle for efficient management of UpdateHealth timer */
	FTimerHandle TimerHandle_ReturnToMenu;
};


