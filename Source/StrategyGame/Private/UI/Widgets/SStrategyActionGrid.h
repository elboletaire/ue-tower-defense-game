// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "StrategyTypes.h"

class AStrategyHUD;


// Grid of actions something can perform
class SStrategyActionGrid : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyActionGrid)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<AStrategyHUD>, OwnerHUD)

	SLATE_END_ARGS()

	/** needed for every widget */
	void Construct(const FArguments& InArgs);
	/** 3 x 3 grid of action buttons */
	TArray< TSharedPtr<FActionButtonInfo> > ActionButtons;

private:
	/** says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** one of the buttons was clicked, trigger action if any */
	FReply TriggerAction(int32 idx) const;
	/** gets cost to display associated with action */
	FText GetActionCostText(int32 idx) const;
	/** gets action text to display, it should display no text if icon is available */
	FText GetActionText(int32 idx) const;
	/** gets action queue length text if queue is supported */
	FText GetActionQueueText(int32 idx) const;
	/** gets action visibility */
	EVisibility GetActionVisibility(int32 idx) const;
	/** gets coin icon visibility - when cost is 0, coin icon is not visible */
	TOptional<EVisibility> GetCoinIconVisibility(int32 idx) const;
	/** gets tooltip text */
	FText GetTooltip(int32 idx) const;
	/** gets action padding, used for getting circle layout */
	FMargin GetActionPadding(int32 idx) const;
	/** gets enabled state of the action */
	bool	GetEnabled(int32 idx) const;

	/** shared pointer to action grid panel */
	TSharedPtr<SGridPanel> ActionGrid;
	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AStrategyHUD> OwnerHUD;
};


