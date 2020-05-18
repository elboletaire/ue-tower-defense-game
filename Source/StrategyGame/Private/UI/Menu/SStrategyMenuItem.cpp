// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategyMenuItem.h"

void SStrategyMenuItem::Construct(const FArguments& InArgs)
{
	 SStrategyButtonWidget::Construct(
		 SStrategyButtonWidget::FArguments()
		.ButtonText(InArgs._Text)
		.TextFont(FStrategyStyle::Get().GetFontStyle(TEXT("StrategyGame.MenuFont")))
		.TextVAlign(EVerticalAlignment::VAlign_Center)
		.TextMargin(FMargin(0))
		.Visibility(EVisibility::Visible)
		.OnClicked(InArgs._OnClicked)
		[
			InArgs._Content.Widget
		]
		);
}