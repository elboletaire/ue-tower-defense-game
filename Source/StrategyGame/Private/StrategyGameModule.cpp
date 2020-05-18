// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"

#include "StrategyHUDSoundsWidgetStyle.h"
#include "StrategyHUDWidgetStyle.h"
#include "StrategyMenuWidgetStyle.h"



class FStrategyGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		//Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FStrategyStyle::GetStyleSetName());
		FStrategyStyle::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FStrategyStyle::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FStrategyGameModule, StrategyGame, "StrategyGame");

DEFINE_LOG_CATEGORY(LogGame)
