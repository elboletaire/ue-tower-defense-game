// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategySlateHUDWidget.h"
#include "SStrategyActionGrid.h"
#include "SStrategyButtonWidget.h"
#include "StrategyHelpers.h"
#include "StrategyHUDSoundsWidgetStyle.h"
#include "StrategyHUDWidgetStyle.h"
#include "StrategyCheatManager.h"

#include "Engine/Console.h"


void SStrategySlateHUDWidget::Construct(const FArguments& InArgs)
{
	HUDStyle = &FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDStyle>("DefaultStrategyHUDStyle");

	OwnerHUD = InArgs._OwnerHUD;
	bIsPauseMenuActive = false;
	PauseMenuButtons.AddZeroed((SupportsQuitButton ? 3 : 2));
	Visibility.Bind(this, &SStrategySlateHUDWidget::GetSlateVisibility);
	UIScale.Bind(this, &SStrategySlateHUDWidget::GetUIScale);
	MiniMapBorderMargin = 20;

	TSharedPtr<SVerticalBox> MenuBox;
	int32 ButtonIndex = 0;
	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SDPIScaler)
		.DPIScale(UIScale)
		[

			SNew(SOverlay)
			+SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SCanvas)
				+SCanvas::Slot()
				.Position(TAttribute<FVector2D>(this,&SStrategySlateHUDWidget::GetActionsWidgetPos))
				.Size(FVector2D(600,800))
				[
					SAssignNew(ActionButtonsWidget,SStrategyActionGrid)
					.OwnerHUD(OwnerHUD)
				]
			]
			+SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Left)
			.Padding(FMargin(MiniMapBorderMargin,0,0,MiniMapBorderMargin))
			[
				SNew(SBorder)
				.BorderImage(&HUDStyle->MinimapFrameBrush)
				.Padding(FMargin(0))
				[
					SNew(SBox)
					.Padding(FMargin(MiniMapBorderMargin,MiniMapBorderMargin))
					.WidthOverride(this,&SStrategySlateHUDWidget::GetMiniMapWidth)
					.HeightOverride(this,&SStrategySlateHUDWidget::GetMiniMapHeight)
					[
						SAssignNew(MiniMapWidget,SStrategyMiniMapWidget)
						.OwnerHUD(OwnerHUD)
					]
				]
			]
			+SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.BorderImage(&HUDStyle->ResourcesBackgroundBrush)
				.Padding(FMargin(30.0f,10.0f))
				[
					SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.WidthOverride(200)
					.HeightOverride(60)
					[
						SNew(SHorizontalBox)
						.Visibility(this, &SStrategySlateHUDWidget::GetResourcesVisibility)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.TextStyle(FStrategyStyle::Get(), "StrategyGame.ResourcesTextStyle")
							.Text(this, &SStrategySlateHUDWidget::GetResourcesAmount)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(SBox)
								.WidthOverride(48)
								.HeightOverride(48)
								[
									SNew(SImage)
									.Image(&HUDStyle->ResourcesImage)
								]
							]
						]
					]
				]
			]
			+SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.TextStyle(FStrategyStyle::Get(), "StrategyGame.ResourcesTextStyle")
				.Text(this, &SStrategySlateHUDWidget::GetGameTime)
			]
			/* Result screen { */
			+SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SImage)
					.Image(this, &SStrategySlateHUDWidget::GetGameResultImage)
					.Visibility(this, &SStrategySlateHUDWidget::GetResultScreenVisibility)
				]

				+SOverlay::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Center)
				[
					SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.WidthOverride(675)
					.HeightOverride(310)
					[
						SNew(STextBlock)
						.Visibility(this, &SStrategySlateHUDWidget::GetResultScreenVisibility)
						.ShadowColorAndOpacity(FLinearColor::Black)
						.ColorAndOpacity(this, &SStrategySlateHUDWidget::GetGameResultColor)
						.ShadowOffset(FIntPoint(-1,1))
						.Font(this, &SStrategySlateHUDWidget::GetGameResultFont)
						.Text(this, &SStrategySlateHUDWidget::GetGameResultText)
					]
				]
			]

			/* Result screen } */
			+SOverlay::Slot() // pause menu background overlay
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SBorder)
				.Visibility(this, &SStrategySlateHUDWidget::GetPauseMenuBgVisibility)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.BorderImage(FCoreStyle::Get().GetBrush("BlackBrush"))
				.BorderBackgroundColor(this, &SStrategySlateHUDWidget::GetOverlayColor)
				[
					SNew(SOverlay)
					// Return to main
					+SOverlay::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SAssignNew(MenuBox, SVerticalBox)
						+SVerticalBox::Slot()
						[
							SAssignNew(PauseMenuButtons[ButtonIndex++], SStrategyButtonWidget)
							.OwnerHUD(OwnerHUD)
							.Visibility(EVisibility::Visible)
							.TextFont(FStrategyStyle::Get().GetFontStyle(TEXT("StrategyGame.MenuFont")))
							.TextVAlign(EVerticalAlignment::VAlign_Center)
							.TextMargin(FMargin(0))
							.ButtonText(NSLOCTEXT("SStrategySlateHUDWidget", "MainMenu", "Main Menu"))
							.OnClicked(this, &SStrategySlateHUDWidget::OnReturnToMainMenu)
						]
					]
					
				]
			]
			+SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			.Padding(FMargin(0,0,20,20))
			[
				SAssignNew(PauseButton,SStrategyButtonWidget)
				.OwnerHUD(OwnerHUD)
				.Visibility(EVisibility::Visible)
				.OnClicked(this, &SStrategySlateHUDWidget::TogglePauseMenu)
				.ButtonText(FText::GetEmpty())
			]
		]
	];

	{
		// Cheats
		MenuBox->AddSlot()
			[
				SAssignNew(PauseMenuButtons[ButtonIndex++], SStrategyButtonWidget)
				.OwnerHUD(OwnerHUD)
				.Visibility(EVisibility::Visible)
				.TextFont(FStrategyStyle::Get().GetFontStyle(TEXT("StrategyGame.MenuFont")))
				.TextVAlign(EVerticalAlignment::VAlign_Center)
				.TextMargin(FMargin(0))
				.ButtonText(NSLOCTEXT("SStrategySlateHUDWidget", "CheatGold", "Cheat-Gold"))
				.OnClicked(this, &SStrategySlateHUDWidget::OnCheatAddGold)
			];	
	}

	if (SupportsQuitButton)
	{
		MenuBox->AddSlot()
		[
			SAssignNew(PauseMenuButtons[ButtonIndex++], SStrategyButtonWidget)
			.OwnerHUD(OwnerHUD)
			.Visibility(EVisibility::Visible)
			.TextFont(FStrategyStyle::Get().GetFontStyle(TEXT("StrategyGame.MenuFont")))
			.TextVAlign(EVerticalAlignment::VAlign_Center)
			.TextMargin(FMargin(0))
			.ButtonText(NSLOCTEXT("SStrategySlateHUDWidget", "Exit", "Exit"))
			.OnClicked(this, &SStrategySlateHUDWidget::OnExitGame)
		];
	}
}

FSlateColor SStrategySlateHUDWidget::GetOverlayColor() const
{
	const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");

	FLinearColor Result(0,0,0,0.3f);
	const float PosExiting = FMath::Max(MenuHelper::GetSoundPlaybackPosition(OwnerHUD->PlayerOwner->GetWorld(),HUDSounds.ExitGameSound, ExitGameTimerHandle),
						     MenuHelper::GetSoundPlaybackPosition(OwnerHUD->PlayerOwner->GetWorld(),HUDSounds.ExitGameSound, ReturnToMainMenuTimerHandle));
	Result.A = 0.3f + 0.7f * PosExiting;
	if (PosExiting > 0.95f)
	{
		OwnerHUD->ShowBlackScreen();
	}
	return Result;
}

FReply SStrategySlateHUDWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().LockMouseToWidget(SharedThis(this));
}

float SStrategySlateHUDWidget::GetUIScale() const
{
	float Result = 0.5f;
	if ( GEngine && GEngine->GameViewport )
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		Result = ViewportSize.X / 2048.0f;
	}
	//as a fall back assume 1024x768
	return Result;
}

void SStrategySlateHUDWidget::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );
	//ugly code seeing if the console is open
	UConsole* ViewportConsole = (GEngine !=NULL && GEngine->GameViewport != NULL) ? GEngine->GameViewport->ViewportConsole : NULL;
	if (ViewportConsole != NULL && (ViewportConsole->ConsoleState == "Typing" || ViewportConsole->ConsoleState == "Open"))
	{
		if (!bConsoleVisible)
		{
			bConsoleVisible = true;
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
		}
	}
	else
	{
		if (bConsoleVisible)
		{
			bConsoleVisible = false;
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		}
	}
}

EVisibility SStrategySlateHUDWidget::GetSlateVisibility() const
{
	return bConsoleVisible ? EVisibility::HitTestInvisible : EVisibility::Visible;
}

FOptionalSize SStrategySlateHUDWidget::GetMiniMapWidth() const
{
	float Result = 0.0f;
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState && MyGameState->MiniMapCamera.IsValid())
	{
		Result = MyGameState->MiniMapCamera->MiniMapWidth;
	}
	return Result;
}

FOptionalSize SStrategySlateHUDWidget::GetMiniMapHeight() const
{
	float Result = 0.0f;
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState && MyGameState->MiniMapCamera.IsValid())
	{
		Result = MyGameState->MiniMapCamera->MiniMapHeight;
	}
	return Result;
}

FCursorReply SStrategySlateHUDWidget::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FText SStrategySlateHUDWidget::GetResourcesAmount() const
{
	const AStrategyPlayerController* const PC = Cast<AStrategyPlayerController>(OwnerHUD->PlayerOwner);
	AStrategyGameState* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState && PC)
	{
		FPlayerData* const PlayerData = MyGameState->GetPlayerData(PC->GetTeamNum());
		if (PlayerData)
		{
			return FText::AsNumber(PlayerData->ResourcesAvailable);
		}
	}
	return FText::GetEmpty();
}

FText SStrategySlateHUDWidget::GetGameTime() const
{
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState != NULL)
	{
		if (MyGameState->GameplayState == EGameplayState::Waiting)
		{
			int32 const DisplaySecondsRemaining = FMath::CeilToInt(MyGameState->GetRemainingWaitTime());
			return FText::Format( NSLOCTEXT("GameFlow", "GameStartsIn", "Game starts in {0}"), FText::AsNumber(DisplaySecondsRemaining) );
		}
	}
	return FText::GetEmpty();
}

FSlateFontInfo SStrategySlateHUDWidget::GetGameResultFont() const
{
	FSlateFontInfo ResultFont;
	const float AnimTime = 1.0f;
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	const float GameFinishedTime = MyGameState ? MyGameState->GetGameFinishedTime() : 0.0f;
	float AnimPercentage = FMath::Min(1.0f, (OwnerHUD->GetWorld()->GetRealTimeSeconds() - GameFinishedTime) / AnimTime);
	if (GameFinishedTime > 0)
	{
		const int32 StartFontSize = 8;
		const int32 AnimatedFontSize = 70;
		ResultFont = FSlateFontInfo( FPaths::ProjectContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), FMath::TruncToInt(StartFontSize + AnimatedFontSize * AnimPercentage));
	}
	else
	{
		ResultFont = FCoreStyle::Get().GetFontStyle(TEXT("NormalFont"));
	}
	return ResultFont;
}

FSlateColor SStrategySlateHUDWidget::GetGameResultColor() const
{
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	return (MyGameState && MyGameState->GetWinningTeam() == EStrategyTeam::Player) ? HUDStyle->VictoryTextColor : HUDStyle->DefeatTextColor;
}

FText SStrategySlateHUDWidget::GetGameResultText() const
{
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState != NULL)
	{
		return MyGameState->GetWinningTeam() == EStrategyTeam::Player ? NSLOCTEXT("GameFlow", "GameWon", "VICTORY") : NSLOCTEXT("GameFlow", "GameLost", "DEFEAT");
	}
	return FText::GetEmpty();
}

const FSlateBrush* SStrategySlateHUDWidget::GetGameResultImage() const
{
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	bool const bVictory = (MyGameState && MyGameState->GetWinningTeam() == EStrategyTeam::Player);
	return bVictory ?  &HUDStyle->VictoryImage : &HUDStyle->DefeatImage;
}

EVisibility SStrategySlateHUDWidget::GetResourcesVisibility() const
{
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	return MyGameState && MyGameState->IsGameActive() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SStrategySlateHUDWidget::GetResultScreenVisibility() const
{
	EVisibility ResultVisibility = EVisibility::Collapsed;
	AStrategyGameState const* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if ( MyGameState && (MyGameState->GameplayState == EGameplayState::Finished) )
	{
		ResultVisibility = EVisibility::Visible;
	}

	return ResultVisibility;
}

EVisibility SStrategySlateHUDWidget::GetPauseMenuBgVisibility() const
{
	return bIsPauseMenuActive ? EVisibility::Visible : EVisibility::Collapsed;
}

FVector2D SStrategySlateHUDWidget::GetActionsWidgetPos() const
{
	return ActionWidgetPosition.Get();
}

FReply SStrategySlateHUDWidget::TogglePauseMenu()
{
	AStrategyGameState* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState != NULL)
	{
		const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
		FSlateApplication::Get().PlaySound(HUDSounds.MenuItemChangeSound);
		bIsPauseMenuActive = !bIsPauseMenuActive;
		//Do not trigger pause when game is already finished
		if (MyGameState->GameplayState != EGameplayState::Finished)
		{
			MyGameState->SetGamePaused(bIsPauseMenuActive);
		}
		for (uint8 i = 0; i < PauseMenuButtons.Num(); i++)
		{
			if (bIsPauseMenuActive)
			{
				PauseMenuButtons[i]->DeferredShow();
			}
			else 
			{
				PauseMenuButtons[i]->DeferredHide();
			}
		}
	}
	return FReply::Handled();
}

bool SStrategySlateHUDWidget::IsPauseMenuUp() const
{
	return bIsPauseMenuActive;
}

FReply SStrategySlateHUDWidget::OnExitGame()
{
	AStrategyGameState* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState)
	{
		MyGameState->SetGamePaused(false);
	}

	for (int32 i = 0; i < PauseMenuButtons.Num(); i++)
	{
		PauseMenuButtons[i]->DeferredHide();
	}
	const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
	ExitGameTimerHandle = MenuHelper::PlaySoundAndCallSP(OwnerHUD->PlayerOwner->GetWorld(), HUDSounds.ExitGameSound, this, &SStrategySlateHUDWidget::ExitGame);
	return FReply::Handled();
}


void SStrategySlateHUDWidget::ExitGame() const
{
	AStrategyGameMode* const MyGame = OwnerHUD->GetWorld()->GetAuthGameMode<AStrategyGameMode>();
	if (MyGame != NULL)
	{
		MyGame->ExitGame();
	}
}

FReply SStrategySlateHUDWidget::OnReturnToMainMenu()
{
	AStrategyGameState* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState)
	{
		MyGameState->SetGamePaused(false);
	}

	for (uint8 i = 0; i < PauseMenuButtons.Num(); i++)
	{
		PauseMenuButtons[i]->DeferredHide();
	}
	const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
	ReturnToMainMenuTimerHandle = MenuHelper::PlaySoundAndCallSP(OwnerHUD->PlayerOwner->GetWorld(), HUDSounds.ExitGameSound, this, &SStrategySlateHUDWidget::ReturnToMainMenu);
	return FReply::Handled();
}

void SStrategySlateHUDWidget::ReturnToMainMenu() const
{
	AStrategyGameMode* const MyGame = OwnerHUD->GetWorld()->GetAuthGameMode<AStrategyGameMode>();
	if (MyGame != NULL)
	{
		MyGame->ReturnToMenu();
	}
}

FReply SStrategySlateHUDWidget::OnCheatAddGold() const
{
	FReply Reply = FReply::Unhandled();
	APlayerController* PlayerController = Cast<APlayerController>(OwnerHUD->GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		UStrategyCheatManager* CheatManager = Cast<UStrategyCheatManager>(PlayerController->CheatManager);
		if (CheatManager != nullptr)
		{
			CheatManager->AddGold(10000);
			Reply = FReply::Handled();
		}
	}
	return Reply;
}

