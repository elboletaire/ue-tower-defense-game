// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class AStrategyHUD;

//class declare
class SStrategyMiniMapWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyMiniMapWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<AStrategyHUD>, OwnerHUD)

	SLATE_END_ARGS()

	/** Needed for every widget */
	void Construct(const FArguments& InArgs);

	/** Says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** mouse button down callback */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	/** mouse button up callback */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	/** mouse move callback */
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	/** mouse leave callback */
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	/** OnPaint override */
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;


private:
	/** holds if mouse button is pressed, resets when mouse leaves the widget */
	bool bIsMouseButtonDown;

	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AStrategyHUD> OwnerHUD;
};
