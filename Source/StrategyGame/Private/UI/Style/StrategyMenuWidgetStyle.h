// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "StrategyMenuWidgetStyle.generated.h"

/**
 * Represents the the appearance of an SStrategyMenuWidget
 */
USTRUCT()
struct FStrategyMenuStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FStrategyMenuStyle();
	virtual ~FStrategyMenuStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FStrategyMenuStyle& GetDefault();

	/**
	 * The brush used to draw the menu background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush BackgroundBrush;
	FStrategyMenuStyle& SetBackgroundBrush(const FSlateBrush& InBackgroundBrush) { BackgroundBrush = InBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the menu tile background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush TileBackgroundBrush;
	FStrategyMenuStyle& SetTileBackgroundBrush(const FSlateBrush& InTileBackgroundBrush) { TileBackgroundBrush = InTileBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the pop-up menu background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush PopupBackgroundBrush;
	FStrategyMenuStyle& SetPopupBackgroundBrush(const FSlateBrush& InPopupBackgroundBrush) { PopupBackgroundBrush = InPopupBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the top menu decoration
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush TopDecoration;
	FStrategyMenuStyle& SetTopDecoration(const FSlateBrush& InTopDecoration) { TopDecoration = InTopDecoration; return *this; }

	/**
	 * The brush used to draw the left menu decoration
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush LeftDecoration;
	FStrategyMenuStyle& SetLeftDecoration(const FSlateBrush& InLeftDecoration) { LeftDecoration = InLeftDecoration; return *this; }

	/**
	 * The brush used to draw the right menu decoration
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush RightDecoration;
	FStrategyMenuStyle& SetRightDecoration(const FSlateBrush& InRightDecoration) { RightDecoration = InRightDecoration; return *this; }

	/**
	 * The sound that should play when entering a sub-menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuEnterSound;
	FStrategyMenuStyle& SetMenuEnterSound(const FSlateSound& InMenuEnterSound) { MenuEnterSound = InMenuEnterSound; return *this; }

	/**
	 * The sound that should play when leaving a sub- menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuExitSound;
	FStrategyMenuStyle& SetMenuExitSound(const FSlateSound& InMenuExitSound) { MenuExitSound = InMenuExitSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UStrategyMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu's appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FStrategyMenuStyle MenuStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &MenuStyle );
	}
};
