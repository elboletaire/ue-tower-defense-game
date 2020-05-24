// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameBlueprintLibrary.generated.h"

class UObject;

UCLASS()
class UStrategyGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	 * Are the two actors on the same team.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on the same team.
	 */
	 UFUNCTION(BlueprintPure, Category=Game)
	static bool AreFriends(AActor* Actor1, AActor* Actor2);

	/**
	 * Are the two actors on different teams.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on different teams.
	 */
	UFUNCTION(BlueprintPure, Category=Game)
	static bool AreEnemies(AActor* Actor1, AActor* Actor2);

	/**
	 * Spawn a projectile.
	 *
	 * @param ProjectileBlueprint	The blueprint of the projectile to spawn.
	 * @param SpawnLocation			Location to spawn projectile.
	 * @param ShootDirection		Direction of travel for the projectile.
	 * @param OwnerTeam				The team that owns the projectile.
	 * @param ImpactDamage			The amount of damage the projectile will do.
	 * @param LifeSpan				The lifespan of the projectile. (Defaults to 10).
	 * @param InOwner				The Strategy building that 'owns' the projectile. (Defaults to NULL).
	 *
	 * @returns a pointer to the spawned projectile or NULL if the spawn was not successful.
	 */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject", DeprecatedFunction, DeprecationMessage="Use SpawnProjectileFromClass"))
	static class AStrategyProjectile* SpawnProjectile(UObject* WorldContextObject, UBlueprint* ProjectileBlueprint,
		const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<EStrategyTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan=10.0f, class AStrategyBuilding* InOwner = NULL);

	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static class AStrategyProjectile* SpawnProjectileFromClass(UObject* WorldContextObject, TSubclassOf<class AStrategyProjectile> ProjectileClass,
		const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<EStrategyTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan=10.0f, class AStrategyBuilding* InOwner = NULL);


	/**
	 * Adds buff for specified strategy character.
	 *
	 * @param InChar			The strategy character to apply the buff values
	 * @param AttackMin			Set the characters Minimal melee attack damage.
	 * @param AttackMax			Sets the characters Maximum melee attack damage.
	 * @param DamageReduction	Amount to reduce the characters damage by.
	 * @param MaxHealthBonus	Amount to increase the characters health by.
	 * @param HealthRegen		Amount to regenerate the characters by.
	 * @param Speed				Amount to adjust the characters Movement speed by.
	 * @param Duration			Duration for the values to apply.
	 * @param bInfiniteDuration	Should this buff apply forever.
	 * @param AttackDistance	Amount to increase the attack distance by.
	 *
	 * @returns true if the actors are on different teams.
	 */

	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void GiveBuff(class AStrategyChar* InChar,
		int32 AttackMin = 5, int32 AttackMax = 10, int32 DamageReduction = 0, int32 MaxHealthBonus = 0, int32 HealthRegen = 10, float Speed = 0.0f, float Duration=30.0f, bool bInfiniteDuration=false, int32 AttackDistance = 10 );

	/**
	 * Give a weapon to specified strategy character.
	 *
	 * @param InChar	The Strategy character to give the weapon to.
	 * @param Armor		The weapon to give the to the character.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use GiveWeaponFromClass"))
	static void GiveWeapon(class AStrategyChar* InChar, UBlueprint* Weapon);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void GiveWeaponFromClass(AStrategyChar* InChar, TSubclassOf<class UStrategyAttachment> WeaponClass);

	/**
	 * Give armor to specified strategy character.
	 *
	 * @param InChar	The Strategy character to give the armor to.
	 * @param Armor		The armor to give the to the character.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use GiveArmorFromClass"))
	static void GiveArmor(class AStrategyChar* InChar, UBlueprint* Armor);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void GiveArmorFromClass(AStrategyChar* InChar, TSubclassOf<class UStrategyAttachment> ArmorClass);

	/**
	 * Toggle visibility of specified minion.
	 *
	 * @param InMinionChar	The minion to toggle the visibilty of.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void ToggleMinionVisibility(class AStrategyChar* InMinionChar);

	/**
	 * Remove specified minion.
	 *
	 * @param InMinionChar	The minion to remove.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn)
	static bool RemoveMinion(class AStrategyChar* InMinionChar);

	/** Deprecated function. Use ShowWaveTitle. */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject", DeprecatedFunction, DeprecationMessage="Please use ShowWaveTitle instead"))
	static void ShowTitle(class UObject* WorldContextObject, FString LocKey);

	/**
	 * Display the wave title.
	 *
	 * @param WaveNumber	The Wave number to show the title for.
	 */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void ShowWaveTitle(class UObject* WorldContextObject, int32 WaveNumber);
};
