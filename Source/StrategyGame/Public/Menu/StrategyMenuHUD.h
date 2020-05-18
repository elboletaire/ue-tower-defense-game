// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyMenuHUD.generated.h"

namespace EMenuAction
{
	enum Type
	{
		GoBack = -1,
		SetEasy = 0,
		SetMedium,
		SetHard
	};
}

UCLASS()
class AStrategyMenuHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
public:

	/** Main menu. */
	TSharedPtr<TArray<class FStrategyMenuItem>> MainMenu;

	/** Currently visible menu. */
	TSharedPtr<TArray<class FStrategyMenuItem>> CurrentMenu;

	/** Menu that is about to open. */
	TSharedPtr<TArray<class FStrategyMenuItem>> NextMenu;

	/** Menu history stack. */
	TArray<TSharedPtr<TArray<class FStrategyMenuItem>>> MenuHistory;

	// Begin HUD interface
	virtual void PostInitializeComponents() override;
	// End HUD interface

	/** 
	 * Clears out the old widgets, rebuilds them
	 *
	 * @param	bHotRelead	Not used.
	 */
	void RebuildWidgets(bool bHotReload = false);

	/** Menu button texture. */
	UPROPERTY()
	UTexture2D* MenuButtonTexture;

private:

	/** Menu widget pointer. */
	TSharedPtr<class SStrategyMenuWidget> MenuWidget;

	/** Selected game difficulty. */
	TEnumAsByte<EGameDifficulty::Type> Difficulty;

	/** 
	 * Helper for adding menu items .
	 *
	 * @param	SubMenu	The item to add.
	 * @param	Text	The item text.
	 */
	FStrategyMenuItem* AddMenuItem(TSharedPtr<TArray<class FStrategyMenuItem>> &SubMenu, FText Text);

	/** Callback for quit button. */
	void ExecuteQuitAction();

	/** 
	 * Callback for start button. 
	 *
	 * @param	Action	Action to take. (Should be EMenuAction type)
	 */
	void ExecuteSelectMapAction(int32 Action);

	/** Starts the game with selected difficulty. */
	void LaunchGame();

	/** Quits the game. */
	void Quit();

	/** Show the loading screen. */
	void ShowLoadingScreen();
};
