// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyGameBlueprintLibrary.h"
#include "SStrategyTitle.h"
#include "StrategyProjectile.h"
#include "StrategyAttachment.h"

UStrategyGameBlueprintLibrary::UStrategyGameBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AStrategyGameMode* GetGameFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(MyWorld);

	AStrategyGameMode* const MyGame = MyWorld->GetAuthGameMode<AStrategyGameMode>();
	return MyGame;
}

AStrategyGameState* GetGameStateFromContextObject(UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(MyWorld);

	AStrategyGameState* const MyGameState = MyWorld->GetGameState<AStrategyGameState>();
	return MyGameState;
}

bool UStrategyGameBlueprintLibrary::AreFriends(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnFriendlyTeam(Actor1, Actor2);
}

bool UStrategyGameBlueprintLibrary::AreEnemies(AActor* Actor1, AActor* Actor2)
{
	return AStrategyGameMode::OnEnemyTeam(Actor1, Actor2);
}

AStrategyProjectile* UStrategyGameBlueprintLibrary::SpawnProjectileFromClass(UObject* WorldContextObject, TSubclassOf<AStrategyProjectile> ProjectileClass,
	const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<EStrategyTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan, AStrategyBuilding* InOwner)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(MyWorld);

	if (*ProjectileClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AStrategyProjectile* const Proj = MyWorld->SpawnActor<AStrategyProjectile>(*ProjectileClass, SpawnLocation, ShootDirection.Rotation(), SpawnInfo);
		if (Proj)
		{
			Proj->Building = InOwner;
			Proj->InitProjectile(ShootDirection, OwnerTeam, ImpactDamage, LifeSpan);
			return Proj;
		}
	}

	return nullptr;
}

AStrategyProjectile* UStrategyGameBlueprintLibrary::SpawnProjectile(UObject* WorldContextObject, UBlueprint* ProjectileBlueprint,
	const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<EStrategyTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan, AStrategyBuilding* InOwner)
{
	return SpawnProjectileFromClass(
		WorldContextObject, 
		ProjectileBlueprint ? *ProjectileBlueprint->GeneratedClass : nullptr,
		SpawnLocation, ShootDirection, OwnerTeam, ImpactDamage, LifeSpan, InOwner);
}

void UStrategyGameBlueprintLibrary::GiveBuff(AStrategyChar* InChar,
	int32 AttackMin, int32 AttackMax, int32 DamageReduction, int32 MaxHealthBonus, int32 HealthRegen, float Speed, float Duration, bool bInfiniteDuration, int32 AttackDistance)
{
	FBuffData NewBuff;
	NewBuff.BuffData.AttackMin = AttackMin;
	NewBuff.BuffData.AttackMax = AttackMax;
	NewBuff.BuffData.DamageReduction = DamageReduction;
	NewBuff.BuffData.MaxHealthBonus = MaxHealthBonus;
	NewBuff.BuffData.HealthRegen = HealthRegen;
	NewBuff.BuffData.Speed = Speed;
	NewBuff.Duration = Duration;
	NewBuff.bInfiniteDuration = bInfiniteDuration;
	NewBuff.BuffData.AttackDistance = AttackDistance;

	if (InChar)
	{
		InChar->ApplyBuff(NewBuff);
	}
}

void UStrategyGameBlueprintLibrary::GiveWeaponFromClass(AStrategyChar* InChar, TSubclassOf<UStrategyAttachment> ArmorClass)
{
	if (InChar && *ArmorClass)
	{
		auto MyWeapon = NewObject<UStrategyAttachment>(InChar, *ArmorClass);
		InChar->SetWeaponAttachment(MyWeapon);
	}
}

void UStrategyGameBlueprintLibrary::GiveWeapon(AStrategyChar* InChar, UBlueprint* WeaponBlueprint)
{
	GiveWeaponFromClass(InChar, WeaponBlueprint ? *WeaponBlueprint->GeneratedClass : nullptr);
}

void UStrategyGameBlueprintLibrary::GiveArmorFromClass(AStrategyChar* InChar, TSubclassOf<UStrategyAttachment> ArmorClass)
{
	if (InChar && *ArmorClass)
	{
		auto MyArmor = NewObject<UStrategyAttachment>(InChar, *ArmorClass);
		InChar->SetArmorAttachment(MyArmor);
	}
}

void UStrategyGameBlueprintLibrary::GiveArmor(AStrategyChar* InChar, UBlueprint* ArmorBlueprint)
{
	GiveArmorFromClass(InChar, ArmorBlueprint ? *ArmorBlueprint->GeneratedClass : nullptr);
}

void UStrategyGameBlueprintLibrary::ToggleMinionVisibility(AStrategyChar* InMinionChar)
{
	if (InMinionChar)
	{
		InMinionChar->SetActorEnableCollision(!InMinionChar->GetActorEnableCollision());
		InMinionChar->SetActorHiddenInGame(!InMinionChar->IsHidden());
	}
}

bool UStrategyGameBlueprintLibrary::RemoveMinion(AStrategyChar* InMinionChar)
{
	if (InMinionChar)
	{
		return InMinionChar->Destroy();
	} 
	else
	{
		return false;
	}
}

void UStrategyGameBlueprintLibrary::ShowTitle(UObject* WorldContextObject, FString LocKey)
{
	//UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	//AStrategyGameMode* const MyGameMode = MyWorld->GetAuthGameMode<AStrategyGameMode>();

	//SAssignNew(MyGameMode->StrategyTitle,SStrategyTitle)
	//.Visibility(EVisibility::HitTestInvisible)
	//.OwnerWorld(MyWorld);
	//MyGameMode->StrategyTitle->ShowTitle(FText::FromString(Localize(TEXT("Titles"), *LocKey, TEXT("StrategyGame"))));
}

void UStrategyGameBlueprintLibrary::ShowWaveTitle(UObject* WorldContextObject, int32 WaveNumber)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	AStrategyGameMode* const MyGameMode = MyWorld->GetAuthGameMode<AStrategyGameMode>();
	if( MyGameMode != nullptr )
	{
		FText Title = FText::Format(NSLOCTEXT("GameFlow", "WaveNr", "Wave {0}"), FText::AsNumber(WaveNumber));

		SAssignNew(MyGameMode->StrategyTitle,SStrategyTitle)
			.Visibility(EVisibility::HitTestInvisible)
			.OwnerWorld(MyWorld);
		MyGameMode->StrategyTitle->ShowTitle(Title);
	}
}
