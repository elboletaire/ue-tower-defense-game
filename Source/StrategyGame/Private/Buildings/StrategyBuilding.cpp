// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyBuilding.h"
#include "GameFramework/PlayerController.h"
#include "SStrategySlateHUDWidget.h"
#include "SStrategyButtonWidget.h"
#include "StrategySelectionInterface.h"


AStrategyBuilding::AStrategyBuilding(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, Cost(0)
	, BuildTime(10)
	, BuildingName(TEXT("Unknown"))
	, Health(100)
	, bAffectFriendlyMinion(true)
	, bAffectEnemyMinion(true)
	, bIsContructionFinished(false)
	, bIsBeingBuild(false)
	, bIsActionMenuDisplayed(false)
	, MyTeamNum(EStrategyTeam::Unknown)
	, RemainingBuildTime(0)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	USceneComponent* const TranslationComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	TranslationComp->Mobility = EComponentMobility::Static;
	RootComponent = TranslationComp;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	TriggerBox->SetVisibility(true);
	//TriggerBox->bVisible = true;
	TriggerBox->bHiddenInGame = true;
	TriggerBox->CastShadow = false;
	TriggerBox->InitBoxExtent(FVector(512, 128, 128));
	TriggerBox->SetRelativeLocation(FVector(512, 0, 128));
	//TriggerBox->RelativeLocation = FVector(512, 0, 128);
	TriggerBox->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
	TriggerBox->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetupAttachment(RootComponent);

	SetCanBeDamaged(false);
	//bCanBeDamaged = false;

	static ConstructorHelpers::FObjectFinder<USoundCue> StartCueObj(TEXT("/Game/Sounds/Interactive_Objects/Building_ConstructionStart_Cue"));
	ConstructionStartStinger = StartCueObj.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> EndCueObj(TEXT("/Game/Sounds/Interactive_Objects/Building_ConstructionFinished_Cue"));
	ConstructionEndStinger = EndCueObj.Object;
}

void AStrategyBuilding::PostLoad()
{
	Super::PostLoad();
	// only do this for BP
	if(IsTemplate() && RootComponent)
	{
		RootComponent->SetRelativeRotation(FRotator::ZeroRotator);
		RootComponent->SetRelativeLocation(FVector::ZeroVector);
	}
}

void AStrategyBuilding::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SpawnTeamNum != EStrategyTeam::Unknown)
	{
		SetTeamNum(SpawnTeamNum);
	}
}

void AStrategyBuilding::Destroyed()
{
	FPlayerData* const PlayerData = GetTeamData();
	if (PlayerData != nullptr)
	{
		PlayerData->BuildingsList.Remove(this);
	}

	Super::Destroyed();
}

void AStrategyBuilding::OnInputTap_Implementation()
{
	if (bIsActionMenuDisplayed)
	{
		HideActionMenu();
	}
	else
	{
		ShowActionMenu();
	}

	if (!bIsBeingBuild)
	{
		OnTapEvent();
	}
}

void AStrategyBuilding::OnInputHold_Implementation()
{
	if (!bIsBeingBuild)
	{
		OnHoldEvent();
	}
}

void AStrategyBuilding::OnInputHoldReleased_Implementation(float TimeDown)
{
	if (!bIsBeingBuild)
	{
		OnHoldReleasedEvent(TimeDown);
	}
}

void AStrategyBuilding::OnInputSwipeUpdate_Implementation(const FVector& DeltaPosition)
{
	if (!bIsBeingBuild)
	{
		OnSwipeUpdateEvent(DeltaPosition);
	}
}

void AStrategyBuilding::OnInputSwipeReleased_Implementation(const FVector& DeltaPosition, float DownTime)
{
	if (!bIsBeingBuild)
	{
		OnSwipeReleasedEvent(DeltaPosition, DownTime);
	}
}

bool AStrategyBuilding::OnSelectionGained_Implementation()
{
	return true;
}

bool AStrategyBuilding::OnSelectionLost_Implementation(const FVector& NewPosition, AActor* NewSelection)
{
	HideActionMenu();
	return true;
}

void AStrategyBuilding::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	AStrategyChar* const OtherChar = Cast<AStrategyChar>(Other);
	if (bIsContructionFinished && CanAffectChar(OtherChar))
	{
		OnCharTouch(OtherChar);
	}
}

bool AStrategyBuilding::CanAffectChar(AStrategyChar const* InChar) const
{
	const bool bIsFriendly = AStrategyGameMode::OnFriendlyTeam(this, InChar);
	return bIsFriendly ? bAffectFriendlyMinion : bAffectEnemyMinion;
}

void AStrategyBuilding::SetTeamNum(uint8 NewTeamNum)
{
	MyTeamNum = NewTeamNum;
	FPlayerData* const PlayerData = GetTeamData();
	if (PlayerData != nullptr)
	{
		PlayerData->BuildingsList.Add(this);
	}
}

FPlayerData* AStrategyBuilding::GetTeamData() const
{
	check( GetWorld() );	
	AStrategyGameState* const StrategyGame = GetWorld()->GetGameState<AStrategyGameState>();
	if( StrategyGame != nullptr )
	{
		return StrategyGame->GetPlayerData(MyTeamNum);
	}
	return nullptr;
}

void AStrategyBuilding::ShowActionMenu()
{
	if (!bIsActionMenuDisplayed && !bIsCustomActionDisplayed)
	{
		UWorld* const World = GetWorld();
		const APlayerController* MyOwner = (MyTeamNum == EStrategyTeam::Player) ? GEngine->GetFirstLocalPlayerController(World) : nullptr;
		AStrategyHUD* const MyHUD = (MyOwner) ? Cast<AStrategyHUD>(MyOwner->GetHUD()) : nullptr;
		if (MyHUD)
		{
			bIsActionMenuDisplayed = true;

			TArray<TSubclassOf<AStrategyBuilding> > UpgradeList;
			GetUpgradeList(UpgradeList);

			const int32 ActionOrder[] = {0,1,2,5,8,6,3};
			MyHUD->HideAllActionButtons(true);
			MyHUD->SetActionGridActor(this);

			for (int32 i = 0; i < UpgradeList.Num(); i++)
			{
				if (UpgradeList[i] != nullptr)
				{
					const AStrategyBuilding* DefBuilding = UpgradeList[i]->GetDefaultObject<AStrategyBuilding>();

					if (i < 6) // max number of actions is 6, 1 action reserved for selling and another one for repair action
					{
						TSharedPtr<FActionButtonInfo> UpgradeAction = MyHUD->GetActionButton(ActionOrder[i]);
						UpgradeAction->Data.Visibility = EVisibility::Visible;
						UpgradeAction->Data.bIsEnabled = true;
						UpgradeAction->Widget->DeferredShow();
						UpgradeAction->Data.ActionCost = DefBuilding->GetBuildingCost(World);
						UpgradeAction->Data.TriggerDelegate.BindUObject(this, &AStrategyBuilding::ReplaceBuilding, UpgradeList[i]);
					
						if (DefBuilding->BuildingIcon != nullptr)
						{
							UpgradeAction->Data.StrButtonText = FText::GetEmpty();
							UpgradeAction->Widget->SetImage(DefBuilding->BuildingIcon);
						} 
						else
						{
							UpgradeAction->Widget->SetImage(MyHUD->DefaultActionTexture);
							UpgradeAction->Data.StrButtonText = FText::FromString(DefBuilding->GetBuildingName());
						}
					}
				}
			}
		}
	}
}

void AStrategyBuilding::HideActionMenu()
{
	if (bIsActionMenuDisplayed || bIsCustomActionDisplayed)
	{
		bIsActionMenuDisplayed = false;
		bIsCustomActionDisplayed = false;

		const APlayerController* MyOwner = (MyTeamNum == EStrategyTeam::Player) ? GEngine->GetFirstLocalPlayerController(GetWorld()) : nullptr;
		AStrategyHUD* const MyHUD = (MyOwner) ? Cast<AStrategyHUD>(MyOwner->GetHUD()) : nullptr;
		if (MyHUD)
		{
			MyHUD->HideAllActionButtons();
		}
	}
}

int32 AStrategyBuilding::GetBuildingCost(UWorld *World) const
{
	int32 BuildingsCounter = 0;

	const APlayerController* MyOwner = GEngine->GetFirstLocalPlayerController(World);
	FPlayerData* const PlayerData = World ? World->GetGameState<AStrategyGameState>()->GetPlayerData(EStrategyTeam::Player) : nullptr;
	if (PlayerData != nullptr)
	{
		for (int32 Idx = 0; Idx < PlayerData->BuildingsList.Num(); Idx++)
		{
			if (PlayerData->BuildingsList[Idx]->GetClass() == this->GetClass())
			{
				BuildingsCounter++;
			}
		}
	}

	return Cost + BuildingsCounter * AdditionalCost;
}

bool AStrategyBuilding::ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass)
{
	AStrategyBuilding* NewBuilding = nullptr;
	return ReplaceBuilding(NewBuildingClass, &NewBuilding);
}

bool AStrategyBuilding::ReplaceBuilding(TSubclassOf<AStrategyBuilding> NewBuildingClass, AStrategyBuilding** OutNewBuilding)
{
	const FPlayerData* MyData = GetTeamData();
	const int32 BuildingCost = NewBuildingClass->GetDefaultObject<AStrategyBuilding>()->GetBuildingCost(GetWorld());

	if (MyData && int32(MyData->ResourcesAvailable) >= BuildingCost ) // max number of actions is 6, 1 action reserved for selling and another one for repair action
	{
		AStrategyBuilding* const NewBuilding = GetWorld()->SpawnActorDeferred<AStrategyBuilding>(NewBuildingClass, GetTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (NewBuilding)
		{
			NewBuilding->SetTeamNum(GetTeamNum());
			UGameplayStatics::FinishSpawningActor(NewBuilding, GetTransform());

			FPlayerData* const TeamData = NewBuilding->GetTeamData();
			check(TeamData);
			if(TeamData != nullptr )
			{
				TeamData->ResourcesAvailable -= BuildingCost;
			}

			AStrategyGameMode const* const MyGame = GetWorld()->GetAuthGameMode<AStrategyGameMode>();
			check(MyGame);
			if( MyGame != nullptr )
			{
				if (NewBuildingClass != MyGame->EmptyWallSlotClass)
				{
					NewBuilding->StartBuild();
				}
			}
			IStrategySelectionInterface::Execute_OnSelectionLost(this, FVector::ZeroVector, nullptr);
			SetLifeSpan( 0.1f );
		}
		*OutNewBuilding = NewBuilding;
		return true;
	}

	return false;
}

bool AStrategyBuilding::StartBuild()
{
	if (bIsContructionFinished)
	{
		return false;
	}

	if (!bIsBeingBuild)
	{
		bIsBeingBuild = true;

		Health = 1;
		InitialBuildTime = RemainingBuildTime = GetBuildTime();
		OnBuildStarted();

		SetActorTickEnabled(true);
		if (ConstructionStartStinger)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ConstructionStartStinger, GetActorLocation());
		}
		return true;
	}

	return false;
}

void AStrategyBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsBeingBuild || bIsContructionFinished)
	{
		return;
	}

	RemainingBuildTime -= DeltaTime;
	if (RemainingBuildTime <= 0 )
	{
		FinishBuild();
	}
	else
	{
		Health = FMath::Min<float>( (1 - (RemainingBuildTime / InitialBuildTime)) * GetMaxHealth(), GetMaxHealth() );
	}
}

void AStrategyBuilding::FinishBuild()
{
	if (bIsBeingBuild)
	{
		bIsBeingBuild = false;
		bIsContructionFinished = true;
		RemainingBuildTime = 0;
		Health = GetMaxHealth();

		if (ConstructionEndStinger)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ConstructionEndStinger, GetActorLocation());
		}
		OnBuildFinished();
		BuildFinishedDelegate.ExecuteIfBound(this);
	}
}

bool AStrategyBuilding::IsBuildFinished()
{
	return bIsContructionFinished;
}

void AStrategyBuilding::GetUpgradeList(TArray<TSubclassOf<AStrategyBuilding> >& UpgradeList) const
{
	for (int32 i = 0; i < Upgrades.Num(); i++)
	{
		UpgradeList.Add(Upgrades[i]);
	}
}

FString AStrategyBuilding::GetBuildingName() const
{
	return BuildingName;
}

int32 AStrategyBuilding::GetBuildTime() const
{
	return BuildTime;
}

uint8 AStrategyBuilding::GetTeamNum() const
{
	return MyTeamNum;
}

int32 AStrategyBuilding::GetHealth() const
{
	return Health;
}

int32 AStrategyBuilding::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AStrategyBuilding>()->GetHealth();
}
