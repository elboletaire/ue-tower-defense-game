// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SStrategyButtonWidget.h"

//class declare
class SStrategyMenuItem : public SStrategyButtonWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyMenuItem)
	{}

	SLATE_DEFAULT_SLOT(FArguments, Content)

	/** Called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_ATTRIBUTE(FText, Text)

	SLATE_END_ARGS()

	/** Needed for every widget */
	void Construct(const FArguments& InArgs);

	/** Says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }
};


