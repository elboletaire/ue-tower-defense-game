// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyTypes.h"

AStrategyGameState::AStrategyGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// team data for: unknown, player, enemy
	PlayersData.AddZeroed(EStrategyTeam::MAX);
	MiniMapCamera = nullptr;
	WinningTeam = EStrategyTeam::Unknown;
	GameFinishedTime = 0;
}

int32 AStrategyGameState::GetNumberOfLivePawns(TEnumAsByte<EStrategyTeam::Type> InTeam) const
{
	return LivePawnCounter[InTeam];
}

void AStrategyGameState::AddChar(AStrategyChar* InChar)
{
	if (InChar != nullptr)
	{
		LivePawnCounter[InChar->GetTeamNum()]++;
	}
}

void AStrategyGameState::RemoveChar(AStrategyChar* InChar)
{
	if (InChar != nullptr)
	{
		LivePawnCounter[InChar->GetTeamNum()]--;
	}
}

void AStrategyGameState::OnCharDied(AStrategyChar* InChar)
{
	if ( InChar && (InChar->GetTeamNum() == EStrategyTeam::Enemy) )
	{
		PlayersData[EStrategyTeam::Player].ResourcesAvailable += InChar->ResourcesToGather;
		RemoveChar(InChar);
	}
}

void AStrategyGameState::OnActorDamaged(AActor* InActor, float Damage, AController* EventInstigator)
{
	IStrategyTeamInterface* const InstigatorTeam = Cast<IStrategyTeamInterface>(EventInstigator);

	// track damage done
	// @todo, this is def not the place for this
	if (InstigatorTeam)
	{
		FPlayerData& TeamData = PlayersData[InstigatorTeam->GetTeamNum()];
		TeamData.DamageDone += FMath::TruncToInt(Damage);
	}
}

void AStrategyGameState::OnCharSpawned(AStrategyChar* InChar)
{
	if ( InChar && !InChar->IsPendingKill() )
	{
		AddChar(InChar);
	}
}

FPlayerData* AStrategyGameState::GetPlayerData(uint8 TeamNum) const
{
	if (TeamNum != EStrategyTeam::Unknown)
	{
		return &PlayersData[TeamNum];
	}

	return nullptr;
}

void AStrategyGameState::SetGameplayState(EGameplayState::Type NewState)
{
	GameplayState = NewState;

	// notify the breweries of the state change
	for (int32 i = 0; i < PlayersData.Num(); i++)
	{
		if (PlayersData[i].Brewery.IsValid())
		{
			PlayersData[i].Brewery->OnGameplayStateChange(NewState);
		}
	}
}

bool AStrategyGameState::IsGameActive() const
{
	return GameplayState == EGameplayState::Playing;
}

float AStrategyGameState::GetRemainingWaitTime() const
{
	if (GameplayState == EGameplayState::Waiting)
	{
		return GetWorldTimerManager().GetTimerRemaining(TimerHandle_OnGameStart);
	}

	return 0.f;
}


EStrategyTeam::Type AStrategyGameState::GetWinningTeam() const
{
	return WinningTeam;
}

void AStrategyGameState::OnGameStart()
{
	SetGameplayState(EGameplayState::Playing);

	WinningTeam = EStrategyTeam::Unknown;
	GameFinishedTime = 0.0f;
}


void AStrategyGameState::FinishGame(EStrategyTeam::Type InWinningTeam)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	SetGameplayState(EGameplayState::Finished);
	WinningTeam = InWinningTeam;
	GameFinishedTime = GetWorld()->GetRealTimeSeconds();
}



void AStrategyGameState::StartGameplayStateMachine()
{
	if (WarmupTime > 0.f)
	{
		SetGameplayState(EGameplayState::Waiting);
		GetWorldTimerManager().SetTimer(TimerHandle_OnGameStart, this, &AStrategyGameState::OnGameStart, WarmupTime, false);
	}
	else
	{
		OnGameStart();
	}
}

void AStrategyGameState::SetTimersPause(bool bIsPaused)
{
	if (GameplayState == EGameplayState::Waiting )
	{
		bIsPaused ? GetWorldTimerManager().PauseTimer(TimerHandle_OnGameStart) : GetWorldTimerManager().UnPauseTimer(TimerHandle_OnGameStart);
	}
}
void AStrategyGameState::SetGamePaused(bool bIsPaused)
{
	AStrategyPlayerController* const MyPlayer = Cast<AStrategyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != nullptr)
	{
		MyPlayer->SetPause(bIsPaused);
		SetTimersPause(bIsPaused);
	}
}

void AStrategyGameState::SetGameDifficulty(EGameDifficulty::Type NewDifficulty)
{
	GameDifficulty = NewDifficulty;
}

float AStrategyGameState::GetGameFinishedTime() const
{
	return GameFinishedTime;
}
