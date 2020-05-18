// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyHUDSoundsWidgetStyle.h"

FStrategyHUDSoundsStyle::FStrategyHUDSoundsStyle()
{
}

FStrategyHUDSoundsStyle::~FStrategyHUDSoundsStyle()
{
}

const FName FStrategyHUDSoundsStyle::TypeName(TEXT("FStrategyHUDSoundsStyle"));

const FStrategyHUDSoundsStyle& FStrategyHUDSoundsStyle::GetDefault()
{
	static FStrategyHUDSoundsStyle Default;
	return Default;
}

void FStrategyHUDSoundsStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UStrategyHUDSoundsWidgetStyle::UStrategyHUDSoundsWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
