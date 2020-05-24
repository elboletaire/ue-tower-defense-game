// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAIController.h"
#include "StrategyAttachment.h"

AStrategyChar::AStrategyChar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, ResourcesToGather(10)
{
	PrimaryActorTick.bCanEverTick = true;

	// no collisions in mesh
	GetMesh()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->UpdatedComponent = GetCapsuleComponent();
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = true;
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = true;
		GetCharacterMovement()->SetJumpAllowed(true);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bAlwaysCheckFloor = false;
	}

	AIControllerClass = AStrategyAIController::StaticClass();
	Health = 100.f;
}

void AStrategyChar::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// initialization
	UpdatePawnData();
	UpdateHealth();
}

bool AStrategyChar::CanBeBaseForCharacter(APawn* Pawn) const
{
	return false;
}

void AStrategyChar::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);

	if (bSelfMoved)
	{
		// if we controlled by an AI, let it know we ran into something
		AStrategyAIController* const AI = Cast<AStrategyAIController>(Controller);
		if (AI)
		{
			AI->NotifyBump(Hit);
		}
	}
}

float AStrategyChar::PlayMeleeAnim()
{
	if ( (Health > 0.f) && MeleeAnim )
	{
		return PlayAnimMontage(MeleeAnim);
	}

	return 0.f;
}

void AStrategyChar::OnMeleeImpactNotify()
{
	const int32 MeleeDamage = FMath::RandRange(ModifiedPawnData.AttackMin, ModifiedPawnData.AttackMax);
	const TSubclassOf<UDamageType> MeleeDmgType = UDamageType::StaticClass();

	// Do a trace to see what we hit
	const float CollisionRadius = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.f;
	const float TraceDistance = CollisionRadius + (ModifiedPawnData.AttackDistance * 1.3f);
	const FVector TraceStart = GetActorLocation();
	const FVector TraceDir = GetActorForwardVector();
	const FVector TraceEnd = TraceStart + TraceDir * TraceDistance;
	TArray<FHitResult> Hits;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MeleeHit), false, this);
	FCollisionResponseParams ResponseParam(ECollisionResponse::ECR_Overlap);
	GetWorld()->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FQuat::Identity, COLLISION_WEAPON, FCollisionShape::MakeBox(FVector(80.f)), TraceParams, ResponseParam);

	for (int32 i=0; i<Hits.Num(); i++)
	{
		FHitResult const& Hit = Hits[i];
		if (AStrategyGameMode::OnEnemyTeam(this, Hit.GetActor()))
		{
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), MeleeDamage, TraceDir, Hit, Controller, this, MeleeDmgType);

			// only damage first hit
			break;
		}
	}
}


float AStrategyChar::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		// no further damage if already dead
		return 0.f;
	}

	// Modify based on game rules.
	AStrategyGameMode* const Game = GetWorld()->GetAuthGameMode<AStrategyGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}

		// broadcast AI-detectable noise
		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);

		// our gamestate wants to know when damage happens
		AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (GameState)
		{
			GameState->OnActorDamaged(this, ActualDamage, EventInstigator);
		}
	}

	return ActualDamage;
}


void AStrategyChar::FellOutOfWorld(const UDamageType& DamageType)
{
	// if we fall out of the world, die
	Die(Health, FDamageEvent(DamageType.GetClass()), nullptr, nullptr);
}


void AStrategyChar::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if ( bIsDying										// already dying
		|| IsPendingKill())								// already destroyed
	{
		return;
	}

	bIsDying = true;
	Health = FMath::Min(0.0f, Health);

	// figure out who killed us
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ?
		Cast<const UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject())
		: GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	// forcibly end any timers that may be in flight
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// notify the game mode if an Enemy dies
	if (GetTeamNum() == EStrategyTeam::Enemy)
	{
		AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (GameState)
		{
			GameState->OnCharDied(this);
		}
	}

	// disable any AI
	AStrategyAIController* const AIController = Cast<AStrategyAIController>(Controller);
	if (AIController)
	{
		AIController->EnableLogic(false);
	}

	// turn off collision
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->BodyInstance.SetResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	}

	// turn off movement
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	// detach the controller
	if (Controller != nullptr)
	{
		Controller->UnPossess();
	}

	// play death animation
	float DeathAnimDuration = 0.f;
	if (DeathAnim)
	{
		DeathAnimDuration = PlayAnimMontage(DeathAnim) / ( GetMesh() && GetMesh()->GlobalAnimRateScale > 0 ? GetMesh()->GlobalAnimRateScale : 1);
		UAnimInstance * AnimInstance = (GetMesh())? GetMesh()->GetAnimInstance() : nullptr;
	}

	// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AStrategyChar::OnDieAnimationEnd, DeathAnimDuration + 0.01, false);
}

void AStrategyChar::OnDieAnimationEnd()
{
	this->SetActorHiddenInGame(true);
	// delete the pawn asap
	SetLifeSpan( 0.01f );
}

void AStrategyChar::SetWeaponAttachment(UStrategyAttachment* Weapon)
{
	if (WeaponSlot != Weapon)
	{
		// detach any existing weapon attachment
		if (WeaponSlot )
		{
			WeaponSlot->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		}

		// attach this one
		WeaponSlot = Weapon;
		if (WeaponSlot )
		{
			WeaponSlot->RegisterComponent();
			WeaponSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSlot->AttachPoint);
			UpdatePawnData();
			UpdateHealth();
		}
	}
}

void AStrategyChar::SetArmorAttachment(UStrategyAttachment* Armor)
{
	if (ArmorSlot != Armor)
	{
		// detach any existing armor attachment
		if (ArmorSlot )
		{
			ArmorSlot->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		}
		// attach this one
		ArmorSlot = Armor;
		if (ArmorSlot )
		{
			ArmorSlot->RegisterComponent();
			ArmorSlot->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ArmorSlot->AttachPoint);
			UpdatePawnData();
			UpdateHealth();
		}
	}
}

bool AStrategyChar::IsWeaponAttached()
{
	return WeaponSlot != nullptr;
}

bool AStrategyChar::IsArmorAttached()
{
	return ArmorSlot != nullptr;
}

uint8 AStrategyChar::GetTeamNum() const
{
	return MyTeamNum;
}

void AStrategyChar::SetTeamNum(uint8 NewTeamNum)
{
	MyTeamNum = NewTeamNum;
}

void AStrategyChar::ApplyBuff(const FBuffData& Buff)
{
	// calc the end time
	FBuffData NewBuff = Buff;
	if (!Buff.bInfiniteDuration)
	{
		NewBuff.EndTime = GetWorld()->GetTimeSeconds() + Buff.Duration;
	}

	// add to active buffs
	ActiveBuffs.Add(NewBuff);

	// update to account for changes
	UpdatePawnData();
	UpdateHealth();
}

void FBuffData::ApplyBuff(struct FPawnData& PawnData)
{
	PawnData.AttackMin += BuffData.AttackMin;
	PawnData.AttackMax += BuffData.AttackMax;
	PawnData.DamageReduction += BuffData.DamageReduction;
	PawnData.HealthRegen += BuffData.HealthRegen;
	PawnData.MaxHealthBonus += BuffData.MaxHealthBonus;
	PawnData.Speed += BuffData.Speed;
}

void AStrategyChar::UpdatePawnData()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeToNextUpdate = -1.f;

	// start from existing base data
	FPawnData NewPawnData = PawnData;

	// add in influence of any active buffs
	for (int32 i = 0; i < ActiveBuffs.Num(); i++)
	{
		// expired buff, clear it out
		if (!ActiveBuffs[i].bInfiniteDuration && CurrentTime >= ActiveBuffs[i].EndTime)
		{
			ActiveBuffs.RemoveAt(i);
			i--;
			continue;
		}

		if (TimeToNextUpdate < 0 || TimeToNextUpdate > ActiveBuffs[i].EndTime - CurrentTime)
		{
			TimeToNextUpdate = ActiveBuffs[i].EndTime - CurrentTime;
		}

		ActiveBuffs[i].ApplyBuff(NewPawnData);
	}

	// add influence of any attachments
	UStrategyAttachment* const InvSlots[] = { WeaponSlot, ArmorSlot };
	for (int32 i = 0; i < ARRAY_COUNT(InvSlots); i++)
	{
		if (InvSlots[i])
		{
			InvSlots[i]->Effect.ApplyBuff(NewPawnData);
		}
	}

	// validate some of our data; only health regen can have negative values
	NewPawnData.AttackMin = FMath::Max(0, NewPawnData.AttackMin);
	NewPawnData.AttackMax = FMath::Max(0, NewPawnData.AttackMax);
	NewPawnData.DamageReduction = FMath::Max(0, NewPawnData.DamageReduction);
	NewPawnData.MaxHealthBonus = FMath::Max(0, NewPawnData.MaxHealthBonus);

	// store the final values
	ModifiedPawnData = NewPawnData;

	// make sure new health doesn't exceed the cap
	Health = FMath::Min<int32>(Health + ModifiedPawnData.MaxHealthBonus, GetMaxHealth());

	// update groundspeed
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.0f, GetClass()->GetDefaultObject<AStrategyChar>()->GetCharacterMovement()->MaxWalkSpeed + NewPawnData.Speed);
	}

	// update the buffs next time any expires, they are also updated when any buff is added
	if (TimeToNextUpdate > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_UpdatePawnData, this, &AStrategyChar::UpdatePawnData, TimeToNextUpdate, false);
	}
}

void AStrategyChar::UpdateHealth()
{
	if ( (Health > 0.f) && (ModifiedPawnData.HealthRegen != 0.f) )
	{
		if (ModifiedPawnData.HealthRegen < 0.f)
		{
			// negative health regen is a DoT, so do the damage
			UGameplayStatics::ApplyDamage(this, -ModifiedPawnData.HealthRegen, Controller, this, UDamageType::StaticClass());
		}
		else
		{
			// add health regen
			Health = FMath::Min<int32>(Health + ModifiedPawnData.HealthRegen, GetMaxHealth());
		}
	}

	// update again in 1 second
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateHealth, this, &AStrategyChar::UpdateHealth, 1.0f, false);
}

const struct FPawnData* AStrategyChar::GetPawnData() const
{
	return &ModifiedPawnData;
}

int32 AStrategyChar::GetHealth() const
{
	return Health;
}

int32 AStrategyChar::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AStrategyChar>()->GetHealth() + ModifiedPawnData.MaxHealthBonus;
}
