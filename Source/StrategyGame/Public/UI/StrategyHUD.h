// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyHUD.generated.h"

UCLASS()
class AStrategyHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:

	// Begin HUD interface
	virtual void DrawHUD() override;
	// End HUD interface

	/** Returns true if the "Pause" Menu up. */
	bool IsPauseMenuUp() const;

	/** 
	 * Hides all the action buttons.
	 * 
	 * @param bInstantHide	Whether to hide immediately or defer
	 */
	void HideAllActionButtons(bool bInstantHide=false);

	/** clears any pending actions (blinking button state) */
	void ClearActionRequiredStates();

	/** 
	 * Gets single action button data.
	 *
	 * @param	Index	Index of the button to get the info for.
	 */
	TSharedPtr<FActionButtonInfo> GetActionButton(int32 Index) const;

	/** gets transparent slate widget covering whole screen */
	TSharedPtr<class SStrategySlateHUDWidget> GetHUDWidget() const;

	/** sets actor for which action grid is displayed */
	void SetActionGridActor(AActor* SelectedActor);

	/** Toggles the in-game pause menu */
	void TogglePauseMenu();

	/** Enables the black screen, used for transition from game */
	void ShowBlackScreen();

	/** position to display action grid */
	FVector2D ActionGridPos;

	/** mini map margin */
	float MiniMapMargin;

	/** default action texture to use */
	UPROPERTY()
	UTexture2D* DefaultActionTexture;

	/** bigger, centered action default texture */
	UPROPERTY()
	UTexture2D* DefaultCenterActionTexture;

	/** minimap frustum points */
	FVector2D MiniMapPoints[4];
	
	/** current UI scale */
	float UIScale;

protected:

	/** draws mouse pointer */
	void DrawMousePointer();

	/** draws mini map */
	void DrawMiniMap();

	/** builds the slate widgets */
	void BuildMenuWidgets();

	/** draw number of lives for player */
	void DrawLives() const;

	/** 
	 * Draws health bar for specific actor.
	 *
	 * @param	ForActor	Actor for which the health bar is for.
	 * @param	HealthPct	Current Health percentage.
	 * @param	BarHeight	Height of the health bar
	 * @param	OffsetY		Y Offset of the health bar.
	 */
	void DrawHealthBar(AActor* ForActor, float HealthPct, int32 BarHeight, int OffsetY = 0) const;

	/** draw health bars for actors */
	void DrawActorsHealth();

	/** gets position to display action grid */
	FVector2D GetActionsWidgetPos() const;

	/** gets player controller */
	class AStrategyPlayerController* GetPlayerController() const;

	/** HUD menu widget */
	TSharedPtr<class SStrategySlateHUDWidget> MyHUDMenuWidget;

	/** actor for which action grid is displayed*/
	TWeakObjectPtr<AActor> SelectedActor;

	/** gray health bar texture */
	UPROPERTY()
	class UTexture2D* BarFillTexture;

	/** player team health bar texture */
	UPROPERTY()
	class UTexture2D* PlayerTeamHPTexture;

	/** enemy team health bar texture */
	UPROPERTY()
	class UTexture2D* EnemyTeamHPTexture;

	/** mouse pointer material (default) */
	UPROPERTY()
	class UMaterial* MousePointerNeutral;

	/** mouse pointer material (attack) */
	UPROPERTY()
	class UMaterial* MousePointerAttack;

	/** Pause button texture */
	UPROPERTY()
	UTexture2D* ActionPauseTexture;

	/** menu button texture */
	UPROPERTY()
	UTexture2D* MenuButtonTexture;

	/** resource texture - gold coin */
	UPROPERTY()
	UTexture2D* ResourceTexture;

	/** lives texture - barrel */
	UPROPERTY()
	UTexture2D* LivesTexture;

	/** if we are currently drawing black screen */
	uint8 bBlackScreenActive : 1;
};

