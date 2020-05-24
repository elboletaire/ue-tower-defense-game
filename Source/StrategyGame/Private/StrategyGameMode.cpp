// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyBuilding.h"
#include "StrategySpectatorPawn.h"
#include "StrategyTeamInterface.h"


AStrategyGameMode::AStrategyGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AStrategyPlayerController::StaticClass();
	SpectatorClass = AStrategySpectatorPawn::StaticClass();
	DefaultPawnClass = AStrategySpectatorPawn::StaticClass();
	GameStateClass = AStrategyGameState::StaticClass();
	HUDClass = AStrategyHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<AStrategyBuilding> EmptyWallSlotHelper(TEXT("/Game/Buildings/Wall/Wall_EmptySlot"));
	EmptyWallSlotClass = EmptyWallSlotHelper.Class;
	if ((GEngine != nullptr) && (GEngine->GameViewport != nullptr))
	{
		GEngine->GameViewport->SetSuppressTransitionMessage(true);
	}
}

// internal
EGameplayState::Type AStrategyGameMode::GetGameplayState() const
{
	return GetGameState<AStrategyGameState>()->GameplayState;
}

const FString AStrategyGameMode::DifficultyOptionName(TEXT("Difficulty"));


void AStrategyGameMode::InitGameState()
{
	Super::InitGameState();

	AStrategyGameState* const StrategyGameState = GetGameState<AStrategyGameState>();
	if (StrategyGameState)
	{
		EGameDifficulty::Type const NewDifficulty = (EGameDifficulty::Type) UGameplayStatics::GetIntOption(OptionsString, DifficultyOptionName, 0);
		StrategyGameState->SetGameDifficulty(NewDifficulty);

		// start the game!
		StrategyGameState->StartGameplayStateMachine();
	}
}

void AStrategyGameMode::RestartPlayer(AController* NewPlayer)
{
	AActor* const StartSpot = FindPlayerStart(NewPlayer);
	if (StartSpot != nullptr)
	{
		// initialize and start it up
		InitStartSpot(StartSpot, NewPlayer);

		AStrategyPlayerController* const NewPC = Cast<AStrategyPlayerController>(NewPlayer);
		if (NewPC != nullptr)
		{
			NewPC->SetInitialLocationAndRotation(StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
		}
	}
	else
	{
		UE_LOG(LogGame, Warning, TEXT("Player start not found, failed to restart player"));
	}
}

float AStrategyGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	// no health changes after game is finished
	if (GetGameplayState() == EGameplayState::Finished)
	{
		return 0.0f;
	}

	if (Damage > 0.f)
	{
		const IStrategyTeamInterface* VictimTeam = Cast<IStrategyTeamInterface>(DamagedActor);
		IStrategyTeamInterface* InstigatorTeam = Cast<IStrategyTeamInterface>(EventInstigator);
		if (InstigatorTeam == nullptr)
		{
			InstigatorTeam = Cast<IStrategyTeamInterface>(DamageCauser);
		}

		// skip friendly fire
		if (InstigatorTeam && VictimTeam && InstigatorTeam->GetTeamNum() == VictimTeam->GetTeamNum())
		{
			return 0.0f;
		}

		// pawn's damage reduction
		const AStrategyChar* DamagedChar = Cast<AStrategyChar>(DamagedActor);
		if (DamagedChar)
		{
			Damage -= DamagedChar->GetPawnData()->DamageReduction;
		}
	}

	return Damage;
}

void AStrategyGameMode::FinishGame(EStrategyTeam::Type InWinningTeam)
{
	AStrategyGameState* CurrentGameState = GetGameState<AStrategyGameState>();
	// If the game state is valid and we are still playing
	if ((CurrentGameState != nullptr) && (CurrentGameState->GameplayState != EGameplayState::Finished) )
	{
		// tell the gamestate to wrap it up
		CurrentGameState->FinishGame(InWinningTeam);

	}
	// Add a timer to return to main if one does not already exist.
	if (GetWorldTimerManager().GetTimerRate(TimerHandle_ReturnToMenu) == -1.0f )
	{
		// set timer to return to the main menu
		GetWorldTimerManager().SetTimer(TimerHandle_ReturnToMenu, this, &AStrategyGameMode::ReturnToMenu, TimeBeforeReturnToMenu, false);
	}
}

void AStrategyGameMode::ReturnToMenu()
{
	GetWorld()->ServerTravel(FString("/Game/Maps/StrategyMenu"));
}

bool AStrategyGameMode::OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const IStrategyTeamInterface* TeamA = Cast<const IStrategyTeamInterface>(ActorA);
	const IStrategyTeamInterface* TeamB = Cast<const IStrategyTeamInterface>(ActorB);

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Unknown) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Unknown))
		return true;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() == TeamB->GetTeamNum());
}

bool AStrategyGameMode::OnEnemyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const IStrategyTeamInterface* TeamA = Cast<const IStrategyTeamInterface>(ActorA);
	const IStrategyTeamInterface* TeamB = Cast<const IStrategyTeamInterface>(ActorB);

	if( (TeamA != nullptr && TeamA->GetTeamNum() == EStrategyTeam::Unknown) || (TeamB != nullptr && TeamB->GetTeamNum() == EStrategyTeam::Unknown))
		return false;

	return (TeamA != nullptr) && (TeamB != nullptr) && (TeamA->GetTeamNum() != TeamB->GetTeamNum());
}


void AStrategyGameMode::ExitGame()
{
	AStrategyPlayerController* PlayerController = nullptr;
	if (GEngine)
	{
		PlayerController = Cast<AStrategyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		PlayerController->ConsoleCommand(TEXT("quit"));
	}
}
