// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SoundDefinitions.h"
#include "StrategyGameState.h"
#include "StrategyChar.h"
#include "StrategyGameMode.h"
#include "StrategyPlayerController.h"
#include "StrategyHUD.h"
#include "StrategyStyle.h"
#include "StrategyGameClasses.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);

/** when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PANCAMERA		ECC_GameTraceChannel3

#if PLATFORM_ANDROID
	#define SupportsQuitButton	false
#elif PLATFORM_IOS || PLATFORM_TVOS
	#define SupportsQuitButton	false
#else
	#define SupportsQuitButton	FPlatformProperties::SupportsQuit()
#endif
