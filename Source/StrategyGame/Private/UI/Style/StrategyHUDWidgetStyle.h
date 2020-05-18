// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "StrategyHUDWidgetStyle.generated.h"

/**
 * Represents the appearance of an SStrategySlateHUDWidget
 */
USTRUCT()
struct FStrategyHUDStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FStrategyHUDStyle();
	virtual ~FStrategyHUDStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FStrategyHUDStyle& GetDefault();

	/**
	 * The brush used to draw the resources area background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush ResourcesBackgroundBrush;
	FStrategyHUDStyle& SetResourcesBackgroundBrush(const FSlateBrush& InResourcesBackgroundBrush) { ResourcesBackgroundBrush = InResourcesBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the resources image
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush ResourcesImage;
	FStrategyHUDStyle& SetResourcesImage(const FSlateBrush& InResourcesImage) { ResourcesImage = InResourcesImage; return *this; }

	/**
	 * The brush used to draw the frame around the mini-map
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush MinimapFrameBrush;
	FStrategyHUDStyle& SetMinimapFrameBrush(const FSlateBrush& InMinimapFrameBrush) { MinimapFrameBrush = InMinimapFrameBrush; return *this; }

	/**
	 * The brush used to draw the victory image
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush VictoryImage;
	FStrategyHUDStyle& SetVictoryImage(const FSlateBrush& InVictoryImage) { VictoryImage = InVictoryImage; return *this; }

	/**
	 * The brush used to draw the defeat image
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush DefeatImage;
	FStrategyHUDStyle& SetDefeatImage(const FSlateBrush& InDefeatImage) { DefeatImage = InDefeatImage; return *this; }

	/**
	 * The color used to draw the victory text
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateColor VictoryTextColor;
	FStrategyHUDStyle& SetVictoryTextColor(const FSlateColor& InVictoryTextColor) { VictoryTextColor = InVictoryTextColor; return *this; }

	/**
	 * The color used to draw the defeat text
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateColor DefeatTextColor;
	FStrategyHUDStyle& SetDefeatTextColor(const FSlateColor& InDefeatTextColor) { DefeatTextColor = InDefeatTextColor; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UStrategyHUDWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the HUD appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FStrategyHUDStyle HUDStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &HUDStyle );
	}
};
