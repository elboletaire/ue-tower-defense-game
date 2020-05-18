// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "StrategyHUDSoundsWidgetStyle.generated.h"

/**
 * Represents the common HUD sounds used in the strategy game
 */
USTRUCT()
struct FStrategyHUDSoundsStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FStrategyHUDSoundsStyle();
	virtual ~FStrategyHUDSoundsStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FStrategyHUDSoundsStyle& GetDefault();

	/**
	 * The sound that should play when starting the game
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound StartGameSound;
	FStrategyHUDSoundsStyle& SetStartGameSound(const FSlateSound& InStartGameSound) { StartGameSound = InStartGameSound; return *this; }

	/**
	 * The sound that should play when exiting the game
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound ExitGameSound;
	FStrategyHUDSoundsStyle& SetExitGameSound(const FSlateSound& InExitGameSound) { ExitGameSound = InExitGameSound; return *this; }

	/**
	 * The sound that should play when changing the selected menu item
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuItemChangeSound;
	FStrategyHUDSoundsStyle& SetMenuItemChangeSound(const FSlateSound& InMenuItemChangeSound) { MenuItemChangeSound = InMenuItemChangeSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UStrategyHUDSoundsWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the sounds */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FStrategyHUDSoundsStyle SoundsStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &SoundsStyle );
	}
};
