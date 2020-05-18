// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "StrategyTypes.h"

DECLARE_DELEGATE_TwoParams(FOnMouseEnter, const FGeometry&, const FPointerEvent&);
DECLARE_DELEGATE_OneParam(FOnMouseLeave, const FPointerEvent&);

class AStrategyHUD;

//Button widget base class
class SStrategyButtonWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyButtonWidget)
	{}

	/* Owning HUD for getting Game World */
	SLATE_ARGUMENT(TWeakObjectPtr<AStrategyHUD>, OwnerHUD)

	SLATE_DEFAULT_SLOT(FArguments, Content)

	/** called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnClicked)

	/** called when the button is clicked, when action is not allowed */
	SLATE_EVENT(FOnClicked, OnClickedDisabled)

	/** called when mouse enters active button area */
	SLATE_EVENT(FOnMouseEnter, OnMouseEnterDel)
	/** called when mouse leaves active button area */
	SLATE_EVENT(FOnMouseLeave, OnMouseLeaveDel)

	/** text on the button */
	SLATE_ATTRIBUTE(FText, ButtonText)
	/** centered text on the button */
	SLATE_ATTRIBUTE(FText, CenterText)
	/** corner text on the button */
	SLATE_ATTRIBUTE(FText, CornerText)
	/** is the coin icon visible on this button */
	SLATE_ATTRIBUTE(TOptional<EVisibility>, CoinIconVisible)

	/** horizontal alignment of the text on the button (default is centered)*/
	SLATE_ATTRIBUTE(TOptional<EHorizontalAlignment>, TextHAlign)
	/** alignment of the text on the button (default is at the bottom) */
	SLATE_ATTRIBUTE(TOptional<EVerticalAlignment>, TextVAlign)
	/** margins of the button text, (default is 10 points bottom margin) */
	SLATE_ATTRIBUTE(TOptional<FMargin>, TextMargin)
	/** font type and size in the button */
	SLATE_ATTRIBUTE(TOptional<FSlateFontInfo>, TextFont)
	/** button opacity */
	SLATE_ATTRIBUTE(TOptional<float>, Opacity)
	/** should button hide mouse when leaving button area */
	SLATE_ATTRIBUTE(TOptional<bool>, HideMouse)
	
	SLATE_END_ARGS()

	/** needed for every widget */
	void Construct(const FArguments& InArgs);

	/** says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }
	/** widget sets up the appropriate mouse settings upon focus */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent);
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

	void SetImage(UTexture2D* Texture);
	void SetActionAllowed(bool bIsAllowed);
	void SetActionActive(bool bIsActive);
	void SetUserActionRequired(bool bIsWaitingForUser);

	void DeferredShow();
	void DeferredHide(bool bInstant = false);
	bool IsAnimating() const;

	/** brush resource that represents a button */
	TSharedPtr<ISlateBrushSource> ButtonImage;
	TSharedPtr<TArray<uint8>> AlphaMap;

protected:
	/** the delegate to execute when the button is clicked */
	FOnClicked OnClicked;
	/** the delegate to execute when the button is clicked, when action is not allowed */
	FOnClicked OnClickedDisabled;
	/** the delegate to execute when mouse enters active button area */
	FOnMouseEnter OnMouseEnterDel;
	/** the delegate to execute when mouse leave active button area */
	FOnMouseLeave OnMouseLeaveDel;

	const FSlateBrush* GetButtonImage() const;
	FSlateColor GetTintColor() const;
	FSlateColor GetImageColor() const;
	FSlateColor GetCoinColor() const;
	FSlateColor GetTextColor() const;
	FLinearColor GetTextShadowColor() const;
	EVisibility	GetCoinVisibility() const;
	FMargin GetTextMargin() const;
	FSlateFontInfo GetTextFont() const;
	float GetCurrentOpacity() const;

	TAttribute<FText> ButtonText;
	TAttribute<FText> CenterText;
	TAttribute<FText> CornerText;
	TAttribute< TOptional<EVisibility> > CoinIconVisible;
	TAttribute< TOptional<EHorizontalAlignment> > TextHAlign;
	TAttribute< TOptional<EVerticalAlignment> >	TextVAlign;
	TAttribute< TOptional<FMargin> > TextMargin;
	TAttribute< TOptional<FSlateFontInfo> >	TextFont;
	TAttribute< TOptional<float> > Opacity;
	TAttribute< TOptional<bool> > HideMouse;

	FCurveSequence WidgetAnimation;
	FCurveHandle OpacityCurve;

	bool bIsMouseButtonDown;
	bool bIsActionAllowed;
	bool bIsActiveAction;
	bool bIsUserActionRequired;
	bool bMouseCursorVisible;

	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AStrategyHUD> OwnerHUD;
};


