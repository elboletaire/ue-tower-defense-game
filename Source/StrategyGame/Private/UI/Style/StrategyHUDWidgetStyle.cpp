// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyHUDWidgetStyle.h"

FStrategyHUDStyle::FStrategyHUDStyle()
{
}

FStrategyHUDStyle::~FStrategyHUDStyle()
{
}

const FName FStrategyHUDStyle::TypeName(TEXT("FStrategyHUDStyle"));

const FStrategyHUDStyle& FStrategyHUDStyle::GetDefault()
{
	static FStrategyHUDStyle Default;
	return Default;
}

void FStrategyHUDStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&ResourcesBackgroundBrush);
	OutBrushes.Add(&ResourcesImage);
	OutBrushes.Add(&MinimapFrameBrush);
	OutBrushes.Add(&VictoryImage);
	OutBrushes.Add(&DefeatImage);
}


UStrategyHUDWidgetStyle::UStrategyHUDWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
