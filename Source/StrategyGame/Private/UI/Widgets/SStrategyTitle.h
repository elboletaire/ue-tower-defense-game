// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

//class declare
class SStrategyTitle : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStrategyTitle)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class UWorld>, OwnerWorld)

	SLATE_END_ARGS()

	/** needed for every widget */
	void Construct(const FArguments& InArgs);

	/** animates title and fades away after some amount of time */
	void ShowTitle(FText InTitleText);

	/** removes widget from viewport */
	void DetachTitle();

	/** returns what cursor we want when on this widget */
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:

	FLinearColor GetTitleColors(bool bShadowColor) const;

	/** returns title color */
	FSlateColor GetTitleColor() const;

	/** returns title shadow color */
	FLinearColor GetTitleShadowColor() const;

	/** returns title font (used for animation) */
	FSlateFontInfo GetTitleFont() const;

	/** returns title string to display */
	FText GetTitleText() const;

	/** returns how long this widget has been alive */
	float GetTimeAlive() const;

	/** stores when title was requested, used for animation */
	float TitleRequestedTime;

	/** how long the widget should be alive */
	float TimeToLive;

	/** how long the widget should take to fade out */
	float FadeOutTime;

	/** current title text */
	FText TitleText;

	/** pointer to active widget container */
	TSharedPtr<SWeakWidget> TitleContainer;

	/** Owner world */
	TWeakObjectPtr<class UWorld> OwnerWorld;

private:

	/** SWidget overrides */
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
};
