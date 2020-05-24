// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyBuilding_Brewery.h"
#include "SStrategyButtonWidget.h"
#include "SStrategySlateHUDWidget.h"
#include "StrategyAIDirector.h"
#include "StrategyBuilding.h"

AStrategyBuilding_Brewery::AStrategyBuilding_Brewery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SpawnCost(20)
	, NumberOfLives(1)
{
	PrimaryActorTick.bCanEverTick = true;

	Health = 1000;
	bIsBeingBuild = false;
	bIsContructionFinished = true;
	bAffectFriendlyMinion = false;
	bAffectEnemyMinion = false;

	AIDirector = CreateDefaultSubobject<UStrategyAIDirector>(TEXT("AIDirectorComp"));
	EmptySlotClass = AStrategyBuilding::StaticClass();
}

void AStrategyBuilding_Brewery::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FPlayerData* const MyData = GetTeamData();
	if (MyData)
	{
		int32 ResourceInitial = 0;

		AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (MyGameState)
		{
			switch (MyGameState->GameDifficulty)
			{
			case EGameDifficulty::Easy:
				ResourceInitial = ResourceInitialEasy;
				break;
			case EGameDifficulty::Medium:
				ResourceInitial = ResourceInitialMedium;
				break;
			case EGameDifficulty::Hard:
				ResourceInitial = ResourceInitialHard;
				break;
			default:
				break;
			}
		}

		MyData->ResourcesAvailable = ResourceInitial;
		MyData->Brewery = this;
	}
}

void AStrategyBuilding_Brewery::OnGameplayStateChange(EGameplayState::Type NewState)
{
	if (AIDirector != nullptr)
	{
		AIDirector->OnGameplayStateChange(NewState);
	}
}

void AStrategyBuilding_Brewery::SetTeamNum(uint8 NewTeamNum)
{
	Super::SetTeamNum(NewTeamNum);

	if (AIDirector != nullptr)
	{
		AIDirector->SetTeamNum(MyTeamNum);
		if (NewTeamNum == EStrategyTeam::Player)
		{
			AIDirector->WaveSize = 0;
		}
	}

}

uint8 AStrategyBuilding_Brewery::GetNumberOfLives() const
{
	return NumberOfLives;
}

void AStrategyBuilding_Brewery::SetNumberOfLives(uint8 InNumberOfLives)
{
	NumberOfLives = InNumberOfLives;
}

bool AStrategyBuilding_Brewery::ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass)
{
	bool RetVal = false;

	if (EmptySlotClass == NULL)
		return RetVal;

	AStrategyBuilding* NewBuilding = NULL;
	if (LeftSlot.IsValid() && LeftSlot->IsA(EmptySlotClass))
	{
		RetVal = LeftSlot->ReplaceBuilding(NewBuildingClass, &NewBuilding);
		Upgrades.Remove( *NewBuildingClass );
	}
	else if (RightSlot.IsValid() && RightSlot->IsA(EmptySlotClass))
	{
		RetVal = RightSlot->ReplaceBuilding(NewBuildingClass, &NewBuilding);
		Upgrades.Remove( *NewBuildingClass );
	}

	if (NewBuilding)
	{
		NewBuilding->BuildFinishedDelegate.BindUObject(this, &AStrategyBuilding_Brewery::OnConstructedBuilding);
		if (UpgradeStinger)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UpgradeStinger, GetActorLocation());
		}
	}

	return RetVal;
}

void AStrategyBuilding_Brewery::OnConstructedBuilding(AStrategyBuilding* ConstructedUpgrade)
{
	OnConstructedUpgrade.Broadcast(ConstructedUpgrade);
}

void AStrategyBuilding_Brewery::ShowActionMenu()
{
	Super::ShowActionMenu();
	if (bIsActionMenuDisplayed)
	{
		const APlayerController* MyOwner = (MyTeamNum == EStrategyTeam::Player) ? GEngine->GetFirstLocalPlayerController(GetWorld()) : NULL;
		AStrategyHUD* const MyHUD = (MyOwner) ? Cast<AStrategyHUD>(MyOwner->GetHUD()) : NULL;
		if (MyHUD)
		{
			TSharedPtr<FActionButtonInfo> const CenterAction = MyHUD->GetActionButton(4);
			CenterAction->Widget->DeferredShow();
			CenterAction->Widget->SetImage(MyHUD->DefaultCenterActionTexture);
			CenterAction->Data.ActionCost = SpawnCost;
			CenterAction->Data.Visibility = EVisibility::Visible;
			CenterAction->Data.GetQueueLengthDelegate.BindUObject(this, &AStrategyBuilding_Brewery::GetSpawnQueueLength);
			CenterAction->Data.TriggerDelegate.BindUObject(this, &AStrategyBuilding_Brewery::SpawnDwarf);
		}
	}

}

FText AStrategyBuilding_Brewery::GetSpawnQueueLength() const
{
	return AIDirector->WaveSize > 0 ? FText::AsNumber(AIDirector->WaveSize) : FText::GetEmpty();
}

bool AStrategyBuilding_Brewery::SpawnDwarf()
{
	FPlayerData* const MyData = GetTeamData();
	if (MyData && int32(MyData->ResourcesAvailable) >= SpawnCost && AIDirector != nullptr)
	{
		AIDirector->RequestSpawn();
		MyData->ResourcesAvailable -= SpawnCost;
	}

	return false;
}
