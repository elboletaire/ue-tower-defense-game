// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyMenuHUD.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SStrategyMenuWidget.h"
#include "StrategyMenuItem.h"
#include "StrategyHelpers.h"
#include "StrategyGameLoadingScreen.h"
#include "StrategyHUDSoundsWidgetStyle.h"


#define LOCTEXT_NAMESPACE "StrategyGame.HUD.Menu"

AStrategyMenuHUD::AStrategyMenuHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MenuButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/MainMenu/MenuButton.MenuButton"), nullptr, LOAD_None, nullptr);

	AddMenuItem(MainMenu,LOCTEXT("Start", "START"));
	AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Easy", "EASY"))->OnConfirmMenuItem.BindUObject(this,&AStrategyMenuHUD::ExecuteSelectMapAction,(int32)EMenuAction::SetEasy);
	AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Normal", "NORMAL"))->OnConfirmMenuItem.BindUObject(this,&AStrategyMenuHUD::ExecuteSelectMapAction,(int32)EMenuAction::SetMedium);
	AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Hard", "HARD"))->OnConfirmMenuItem.BindUObject(this,&AStrategyMenuHUD::ExecuteSelectMapAction,(int32)EMenuAction::SetHard);
	AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Back", "BACK"))->OnConfirmMenuItem.BindUObject(this,&AStrategyMenuHUD::ExecuteSelectMapAction,(int32)EMenuAction::GoBack);

	if (SupportsQuitButton)
	{
		AddMenuItem(MainMenu, LOCTEXT("Quit", "QUIT"))->OnConfirmMenuItem.BindUObject(this, &AStrategyMenuHUD::ExecuteQuitAction);
	}
	CurrentMenu = MainMenu;

}

FStrategyMenuItem* AStrategyMenuHUD::AddMenuItem(TSharedPtr<TArray<class FStrategyMenuItem>> &SubMenu, FText Text)
{
	if (!SubMenu.IsValid())
	{
		SubMenu = MakeShareable(new TArray<FStrategyMenuItem>());
	}
	FStrategyMenuItem MenuItem(Text);
	SubMenu->Add(MenuItem);
	return &SubMenu->Last();
}

void AStrategyMenuHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	//Now that we are here, build our menu widget
	RebuildWidgets();
}

void AStrategyMenuHUD::ExecuteQuitAction()
{
	MenuWidget->HideMenu();
	const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
	MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),HUDSounds.ExitGameSound,this, &AStrategyMenuHUD::Quit);
	MenuWidget->LockControls(true);
}

void AStrategyMenuHUD::Quit()
{
	GetOwningPlayerController()->ConsoleCommand("quit");
}

void AStrategyMenuHUD::ExecuteSelectMapAction(int32 index)
{
	switch (index)
	{
		case EMenuAction::GoBack:
			MenuWidget->MenuGoBack();
			return;
		case EMenuAction::SetEasy:
			Difficulty = EGameDifficulty::Easy;
			break;
		case EMenuAction::SetMedium:
			Difficulty = EGameDifficulty::Medium;
			break;
		case EMenuAction::SetHard:
			Difficulty = EGameDifficulty::Hard;
			break;
		default:
			return;
	}

	MenuWidget->HideMenu();
	MenuWidget->LockControls(true);
	const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
	MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),HUDSounds.StartGameSound,this,&AStrategyMenuHUD::LaunchGame);
}

void AStrategyMenuHUD::LaunchGame()
{
	FString StartStr = FString::Printf(TEXT("/Game/Maps/TowerDefenseMap?%s=%d"), *AStrategyGameMode::DifficultyOptionName, (uint8) Difficulty);
	GetWorld()->ServerTravel(StartStr);
	ShowLoadingScreen();
}

void AStrategyMenuHUD::RebuildWidgets(bool bHotReload)
{
	MenuWidget.Reset();

	if (GEngine && 
		GEngine->GameViewport)
	{
		UGameViewportClient* GVC = GEngine->GameViewport;
		SAssignNew(MenuWidget, SStrategyMenuWidget)
			.Cursor(EMouseCursor::Default)
			.MenuHUD(MakeWeakObjectPtr(this));

		GVC->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(MenuWidget.ToSharedRef()));
	}
}

void AStrategyMenuHUD::ShowLoadingScreen()
{
	IStrategyGameLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IStrategyGameLoadingScreenModule>("StrategyGameLoadingScreen");
	if( LoadingScreenModule != nullptr )
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}
}

#undef LOCTEXT_NAMESPACE