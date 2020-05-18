// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleInterface.h"


/** Module interface for this game's loading screens */
class IStrategyGameLoadingScreenModule : public IModuleInterface
{
public:
	/** Kicks off the loading screen for in game loading (not startup) */
	virtual void StartInGameLoadingScreen() = 0;
};
