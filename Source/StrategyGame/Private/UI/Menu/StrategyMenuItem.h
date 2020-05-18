// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"

class FStrategyMenuItem
{
public:

	/** menu item text */
	FText Text;

	/** sub menu if present */
	TSharedPtr<TArray<FStrategyMenuItem>> SubMenu;

	/** shared pointer to actual slate widget representing the menu item */
	TSharedPtr<class SStrategyMenuItem> Widget;

	/** confirm menu item delegate */
	DECLARE_DELEGATE(FOnConfirmMenuItem);

	/** delegate, which is executed by SShooterMenuWidget if user confirms this menu item */
	FOnConfirmMenuItem OnConfirmMenuItem;

	/** constructor accepting menu item text */
	FStrategyMenuItem(FText _text)
	{
		Text = _text;
	}
};
