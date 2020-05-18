// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyMenuWidgetStyle.h"

FStrategyMenuStyle::FStrategyMenuStyle()
{
}

FStrategyMenuStyle::~FStrategyMenuStyle()
{
}

const FName FStrategyMenuStyle::TypeName(TEXT("FStrategyMenuStyle"));

const FStrategyMenuStyle& FStrategyMenuStyle::GetDefault()
{
	static FStrategyMenuStyle Default;
	return Default;
}

void FStrategyMenuStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
	OutBrushes.Add(&TileBackgroundBrush);
	OutBrushes.Add(&PopupBackgroundBrush);
	OutBrushes.Add(&TopDecoration);
	OutBrushes.Add(&LeftDecoration);
	OutBrushes.Add(&RightDecoration);
}


UStrategyMenuWidgetStyle::UStrategyMenuWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
