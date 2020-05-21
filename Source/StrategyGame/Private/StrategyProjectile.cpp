// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyProjectile.h"

AStrategyProjectile::AStrategyProjectile(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, Building(NULL)
	, ConstantDamage(false)
{
	bInitialized = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCanEverAffectNavigation(false);
	RootComponent = CollisionComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->ProjectileGravityScale = 0.0f;

	DamageType = UDamageType::StaticClass();
}

void AStrategyProjectile::InitProjectile(const FVector& Direction, uint8 InTeamNum, int32 ImpactDamage, float InLifeSpan)
{
	MovementComp->OnProjectileStop.AddDynamic(this, &AStrategyProjectile::OnHit);
	MovementComp->Velocity = MovementComp->InitialSpeed * Direction;
	
	MyTeamNum = InTeamNum;
	RemainingDamage = ImpactDamage;
	SetLifeSpan( InLifeSpan );

	bInitialized = true;
}

void AStrategyProjectile::NotifyActorBeginOverlap(class AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bInitialized)
	{
		return;
	}

	if (HitActors.Find(OtherActor) != INDEX_NONE)
	{
		return;
	}
	HitActors.AddUnique(OtherActor);

	const AStrategyChar* HitChar = Cast<AStrategyChar>(OtherActor);
	if (HitChar && HitChar->GetTeamNum() > EStrategyTeam::Unknown && HitChar->GetTeamNum() != GetTeamNum())
	{
		FHitResult PawnHit;
		PawnHit.Actor = MakeWeakObjectPtr(const_cast<AStrategyChar*>(HitChar));
		PawnHit.Component = HitChar->GetCapsuleComponent();
		PawnHit.bBlockingHit = true;
		PawnHit.Location = PawnHit.ImpactPoint = GetActorLocation();
		PawnHit.Normal = PawnHit.ImpactNormal = -MovementComp->Velocity.GetSafeNormal();

		OnHit(PawnHit);
	}
}

void AStrategyProjectile::OnHit(FHitResult const& HitResult)
{
	DealDamage(HitResult);
	OnProjectileHit(HitResult.Actor.Get(), HitResult.ImpactPoint, HitResult.ImpactNormal);

	if (RemainingDamage <= 0)
	{
		OnProjectileDestroyed();
		Destroy();
	}
}

void AStrategyProjectile::DealDamage(FHitResult const& HitResult)
{
	const AStrategyChar* HitChar = Cast<AStrategyChar>(HitResult.Actor.Get());
	const float PrevHealth = HitChar ? HitChar->Health : 0.0f;

	UGameplayStatics::ApplyPointDamage(HitResult.Actor.Get(), RemainingDamage, -HitResult.ImpactNormal, HitResult, NULL, this, UDamageType::StaticClass());

	if (HitChar && !ConstantDamage)
	{
		RemainingDamage -= FMath::TruncToInt(PrevHealth - HitChar->Health);
	}
}

void AStrategyProjectile::LifeSpanExpired()
{
	OnProjectileDestroyed();

	Super::LifeSpanExpired();
}

uint8 AStrategyProjectile::GetTeamNum() const
{
	return MyTeamNum;
}

void AStrategyProjectile::FellOutOfWorld(const UDamageType& dmgType)
{
	// If we fall out of the world we need to follow the same chain of events as if we hit something. BUT the blueprint destroys us so we dont wanna do that 
	// instead turn off what we need too and call the destroyed event and let the blueprint do what it needs to.
	// (This isn't an ideal way of doing this !)
	DisableComponentsSimulatePhysics();
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	OnProjectileDestroyed();
	SetLifeSpan(0.0f);
}

void AStrategyProjectile::PostLoad()
{
	Super::PostLoad();
	// only do this for BP
	if(IsTemplate() && RootComponent)
	{
		RootComponent->SetRelativeRotation(FRotator::ZeroRotator);
		RootComponent->SetRelativeLocation(FVector::ZeroVector);
	}
}
