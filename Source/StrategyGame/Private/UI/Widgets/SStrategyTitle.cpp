// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategyTitle.h"

void SStrategyTitle::Construct(const FArguments& InArgs)
{
	OwnerWorld = InArgs._OwnerWorld;
	TitleRequestedTime = 0.0f;
	TimeToLive = 2.0f;
	FadeOutTime = 0.2f;

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.ShadowColorAndOpacity(this, &SStrategyTitle::GetTitleShadowColor)
			.ShadowOffset(FIntPoint(-1,1))

			.ColorAndOpacity(this, &SStrategyTitle::GetTitleColor)
			.Font(this, &SStrategyTitle::GetTitleFont)
			.Text(this, &SStrategyTitle::GetTitleText)
		]
	];
}

FSlateFontInfo SStrategyTitle::GetTitleFont() const
{
	FSlateFontInfo ResultFont;
	const int32 StartFontSize = 8;
	const int32 AnimatedFontSize = 70;
	const float AnimTime = 1.0f;
	float AnimPercentage = FMath::Min(1.0f, GetTimeAlive() / AnimTime);

	ResultFont = FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), FMath::TruncToInt(StartFontSize + AnimatedFontSize * AnimPercentage));

	return ResultFont;
}

FText SStrategyTitle::GetTitleText() const
{
	return TitleText;
}

FLinearColor SStrategyTitle::GetTitleColors(bool bShadowColor) const
{
	const float AnimTime = 0.3f;
	float AnimPercentage = 0.0f;

	const float TimeAlive = GetTimeAlive();
	if (TimeAlive < TimeToLive)
	{
		AnimPercentage = FMath::Min(1.0f, TimeAlive / AnimTime);
	}
	else
	{
		AnimPercentage = 1.0f - FMath::Min(1.0f, (TimeAlive - TimeToLive) / FadeOutTime);
	}
	FLinearColor ResultColor = bShadowColor ? FLinearColor::Black : FLinearColor(FColor(255,213,160));
	ResultColor.A = AnimPercentage;

	if (OwnerWorld != nullptr)
	{
		const APlayerController* MyOwner = GEngine->GetFirstLocalPlayerController(OwnerWorld.Get());
		AStrategyHUD* const MyHUD = (MyOwner) ? Cast<AStrategyHUD>(MyOwner->GetHUD()) : nullptr;
		if (MyHUD)
		{
			if (MyHUD->IsPauseMenuUp() == true)
			{
				ResultColor.A = 0.0f;
			}
		}
	}
	return ResultColor;
}

void SStrategyTitle::DetachTitle()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(TitleContainer.ToSharedRef());
	}
}

FSlateColor SStrategyTitle::GetTitleColor() const
{
	return GetTitleColors(false);
}

FLinearColor SStrategyTitle::GetTitleShadowColor() const
{
	return GetTitleColors(true);
}

void SStrategyTitle::ShowTitle(FText InTitleText)
{
	TitleText = InTitleText;
	TitleRequestedTime = OwnerWorld->GetTimeSeconds();

	GEngine->GameViewport->AddViewportWidgetContent(
		SAssignNew(TitleContainer,SWeakWidget)
		.PossiblyNullContent(SharedThis(this))
		);
}

FCursorReply SStrategyTitle::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}

void SStrategyTitle::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	// Call parent implementation
	SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime);

	const float TotalLifespan = (TimeToLive + FadeOutTime);
	if (TitleRequestedTime > 0.0f && GetTimeAlive() >= TotalLifespan)
	{
		DetachTitle();
	}
}

float SStrategyTitle::GetTimeAlive() const
{
	if (!OwnerWorld.IsValid())
		return 0.0f;

	return OwnerWorld->GetTimeSeconds() - TitleRequestedTime;
}
