// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class AStrategyMenuHUD;

//Base menu widget
class SStrategyMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyMenuWidget)
	: _MenuHUD()
	{}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<AStrategyMenuHUD>, MenuHUD)

	/** always goes here */
	SLATE_END_ARGS()

	/** every widget needs a construction function */
	void Construct(const FArguments& InArgs);

	/** Update function. Kind of a hack. Allows us to only start fading in once we are done loading. */
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

	/** mouse down on the menu widget skips animation*/
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** the menu sets up the appropriate mouse settings upon focus */
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	/** setups animation lengths, start points and launches initial animations*/
	void SetupAnimations();

	/** builds left menu panel*/
	void BuildLeftPanel(bool bGoingBack);

	/** builds inactive next menu panel (current selections submenu preview)*/
	void BuildRightPanel();

	/** starts animations to enter submenu, it will become active menu*/
	void EnterSubMenu();

	/** starts reverse animations to go one level up in menu hierarchy*/
	void MenuGoBack();

	/** call to hide menu */
	void HideMenu();

	/** disable/enable menu controls */
	void LockControls(bool bEnable);

	/** if console is currently opened */
	bool bConsoleVisible;

private:

	/** sets hit test invisibility when console is up */
	EVisibility GetVisibility() const;

	/** gets ui scale */
	float GetUIScale() const;

	/** getters used for animating the menu */
	FVector2D GetBottomScale() const;
	FLinearColor GetBottomColor() const;
	FLinearColor GetTopColor() const;
	FSlateColor GetButtonColor(int index) const;
	FMargin GetMenuOffset() const;
	FMargin GetLeftMenuOffset() const;

	/** computes & gets top decor position */
	FVector2D GetTopDecorPosition() const;

	/** gets top decor size */
	FVector2D GetTopDecorSize() const;

	/** callback for when one of the N buttons is clicked */
	FReply ButtonClicked(int32 ButtonIndex);

	/** This function starts the entire fade in process */
	void FadeIn();

	/** our curve sequence and the related handles */
	FCurveSequence MenuWidgetAnimation;

	/** used for menu background scaling animation at the beginning */ 
	FCurveHandle BottomScaleYCurve;

	/** used for main menu logo fade in animation at the beginning  */
	FCurveHandle TopColorCurve;

	/** used for menu background fade in animation at the beginning */
	FCurveHandle BottomColorCurve;

	/** used for menu buttons slide in animation at the beginning */
	FCurveHandle ButtonsPosXCurve;

	/** current menu transition animation sequence */
	FCurveSequence LeftMenuWidgetAnimation;

	/** current menu transition animation curve */
	FCurveHandle LeftMenuScrollOutCurve;

	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AStrategyMenuHUD> MyMenuHUD;

	/** Editable text widget */
	TSharedPtr< SEditableTextBox > InputText;

	/** selected index of current menu */
	int32 SelectedIndex;

	/** left panel animating flag */
	uint8 bLeftMenuChanging : 1;

	/** going back to previous menu animation flag */
	uint8 bGoingBack : 1;

	/** if menu is currently locked */
	uint8 bControlsLocked : 1;

	/** flag when playing hiding animation */
	uint8 bMenuHiding : 1;
	
	/** menu that will override current one after transition animation */
	TSharedPtr<TArray<class FStrategyMenuItem>> PendingLeftMenu;

	/** left(current) menu layout box */
	TSharedPtr<SVerticalBox> LeftBox;

	/** top decor image widget */
	TSharedPtr<SImage> TopDecorImage;

	/** menu padding size X*/
	float MenuPaddingX;

	/** menu padding size Y*/
	float MenuPaddingY;

	/** x offset from center*/
	float MenuOffsetX;

	/** y offset from center*/
	float MenuOffsetY;

	/** current UI scale attribute */
	TAttribute<float> UIScale;

	/** style for this menu */
	const struct FStrategyMenuStyle *MenuStyle;
};