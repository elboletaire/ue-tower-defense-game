// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTeamInterface.h"
#include "StrategyProjectile.generated.h"

class AActor;
class AStrategyBuilding;

// Base class for the projectiles in the game
UCLASS(Blueprintable)
class AStrategyProjectile : public AActor, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

private:
	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;
public:

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Projectile)
	AStrategyBuilding* Building;

	/** if set, damage will be constant through lifespan */
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	bool ConstantDamage;

	/** blueprint event: projectile hit something */
	UFUNCTION(BlueprintImplementableEvent, Category=Projectile)
	void OnProjectileHit(AActor* HitActor, const FVector& HitLocation, const FVector& HitNormal);

	/** blueprint event: projectile hit something */
	UFUNCTION(BlueprintImplementableEvent, Category=Projectile)
	void OnProjectileDestroyed();

	/** initial setup */
	void InitProjectile(const FVector& ShootDirection, uint8 InTeamNum, int32 ImpactDamage, float InLifeSpan);

	/** handle hit */
	UFUNCTION()
	void OnHit(const FHitResult& HitResult);

	// Begin Actor interface

	virtual void LifeSpanExpired() override;

	/** handle touch to detect enemy pawns */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/* Handle falling out of the world */
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	virtual void PostLoad() override;
	// End Actor interface


	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	virtual uint8 GetTeamNum() const override;

	// End StrategyTeamInterface interface

protected:
	/** deal damage */
	void DealDamage(FHitResult const& HitResult);

	/** current team number */
	uint8 MyTeamNum;

	/** remaining damage value */
	int32 RemainingDamage;

	/** list of just hit actors */
	TArray<AActor*> HitActors;

	/** true, if projectile was initialized */
	bool bInitialized;

public:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
};
