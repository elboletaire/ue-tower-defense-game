// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategyButtonWidget.h"
#include "StrategyHelpers.h"
#include "Slate/DeferredCleanupSlateBrush.h"

void SStrategyButtonWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHUD;
	ButtonText = InArgs._ButtonText;
	CenterText = InArgs._CenterText;
	CornerText = InArgs._CornerText;
	OnClicked = InArgs._OnClicked;
	OnClickedDisabled = InArgs._OnClickedDisabled;
	CoinIconVisible = InArgs._CoinIconVisible;
	TextHAlign = InArgs._TextHAlign; 
	TextVAlign = InArgs._TextVAlign; 
	TextMargin = InArgs._TextMargin;
	TextFont = InArgs._TextFont;
	Opacity = InArgs._Opacity;
	bIsUserActionRequired = false;
	bIsMouseButtonDown = false;
	bIsActiveAction = false;
	bIsActionAllowed = true;
	OnMouseEnterDel = InArgs._OnMouseEnterDel;
	OnMouseLeaveDel = InArgs._OnMouseLeaveDel;

	OpacityCurve = WidgetAnimation.AddCurve(0.0f, 0.2f, ECurveEaseFunction::QuadInOut);
	bMouseCursorVisible = true;


	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SStrategyButtonWidget::GetButtonImage)
			.ColorAndOpacity(this,&SStrategyButtonWidget::GetImageColor)
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SStrategyButtonWidget::GetButtonImage)
			.ColorAndOpacity(this,&SStrategyButtonWidget::GetTintColor)
		]
		+SOverlay::Slot()
		.HAlign(TextHAlign.Get().IsSet() ? TextHAlign.Get().GetValue() : EHorizontalAlignment::HAlign_Center)
		.VAlign(TextVAlign.Get().IsSet() ? TextVAlign.Get().GetValue() : EVerticalAlignment::VAlign_Bottom)
		.Padding(TAttribute<FMargin>(this, &SStrategyButtonWidget::GetTextMargin))
		[
			SNew(STextBlock)
			.ShadowColorAndOpacity(this,&SStrategyButtonWidget::GetTextShadowColor)
			.ColorAndOpacity(this,&SStrategyButtonWidget::GetTextColor)
			.ShadowOffset(FIntPoint(-1,1))
			.Font(this, &SStrategyButtonWidget::GetTextFont)
			.Text(ButtonText)
		]
		+SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.ShadowColorAndOpacity(this,&SStrategyButtonWidget::GetTextShadowColor)
			.ColorAndOpacity(this,&SStrategyButtonWidget::GetTextColor)
			.ShadowOffset(FIntPoint(-1,1))
			.Font(this, &SStrategyButtonWidget::GetTextFont)
			.Text(CenterText)
		]
		+SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			SNew(STextBlock)
			.ShadowColorAndOpacity(this,&SStrategyButtonWidget::GetTextShadowColor)
			.ColorAndOpacity(this,&SStrategyButtonWidget::GetTextColor)
			.ShadowOffset(FIntPoint(-1,1))
			.Text(CornerText)
		]
		+SOverlay::Slot()
		[
			InArgs._Content.Widget
		]
		
	];
}

float SStrategyButtonWidget::GetCurrentOpacity() const
{
	 return (Opacity.Get().IsSet() ? Opacity.Get().GetValue() : 1.0f) * FMath::Lerp(0.0f,1.0f,OpacityCurve.GetLerp());
}

FSlateColor	SStrategyButtonWidget::GetImageColor() const
{
	return FLinearColor(1,1,1, GetCurrentOpacity() );
}

FSlateColor	SStrategyButtonWidget::GetCoinColor() const
{
	return FLinearColor(1,1,1, GetCurrentOpacity() );
}

FSlateColor	SStrategyButtonWidget::GetTextColor() const
{
	const FLinearColor GoldColor = FLinearColor(FColor(255,213,160,GetCurrentOpacity()*255));
	return GoldColor;
}

FLinearColor SStrategyButtonWidget::GetTextShadowColor() const
{
	return FLinearColor(0,0,0, GetCurrentOpacity() );
}

EVisibility SStrategyButtonWidget::GetCoinVisibility() const
{
	return CoinIconVisible.Get().IsSet() ? CoinIconVisible.Get().GetValue() : EVisibility::Collapsed;
}

FMargin	SStrategyButtonWidget::GetTextMargin() const
{
	return TextMargin.Get().IsSet() ? TextMargin.Get().GetValue() : FMargin(0,0,0,15);
}

FSlateFontInfo SStrategyButtonWidget::GetTextFont() const
{
	return TextFont.Get().IsSet() ? TextFont.Get().GetValue() : FStrategyStyle::Get().GetFontStyle("StrategyGame.ButtonFont");
}

const FSlateBrush*  SStrategyButtonWidget::GetButtonImage() const
{
	if (ButtonImage.IsValid())
	{
		return ButtonImage.Get()->GetSlateBrush();
	} 
	else 
	{
		return FStrategyStyle::Get().GetBrush("DefaultActionImageBrush");
	}
}

void SStrategyButtonWidget::SetImage(UTexture2D* Texture)
{
	if (Texture != NULL)
	{
		ButtonImage.Reset();
		ButtonImage = FDeferredCleanupSlateBrush::CreateBrush(Texture, FVector2D(Texture->GetSizeX(), Texture->GetSizeY()));
		AlphaMap.Reset();
		AlphaMap = FStrategyHelpers::CreateAlphaMapFromTexture(Texture);
	}
}

void SStrategyButtonWidget::SetActionAllowed(bool bEnabled)
{
	bIsActionAllowed = bEnabled;
}

void SStrategyButtonWidget::SetActionActive(bool bIsActive)
{
	bIsActiveAction = bIsActive;
}

void SStrategyButtonWidget::SetUserActionRequired(bool bIsWaitingForUser)
{
	bIsUserActionRequired = bIsWaitingForUser;
}


FSlateColor SStrategyButtonWidget::GetTintColor() const
{
	FLinearColor ResultTint(0.f,0.f,0.f,0.f);
	FLinearColor NotAllowedTint(0.6f,0.13f,0.06f,0.6f);
	FLinearColor UserInputTint(0.0f,0.6f,0.0f,0.3f);
	FLinearColor PressedTint(0.1f,0.1f,0.1f,0.5f);
	FLinearColor ActiveTint(0.0f,0.5f,0.0f,0.8f);

	if (!bIsActionAllowed)
	{
		ResultTint += NotAllowedTint;
	}
	if (bIsMouseButtonDown)
	{
		ResultTint += PressedTint;
	}
	if (bIsActiveAction)
	{
		ResultTint += ActiveTint;
	}
	if (bIsUserActionRequired)
	{
		ResultTint += UserInputTint;
		if (OwnerHUD.IsValid())
		{
			ResultTint.A = (1.0f+FMath::Sin(OwnerHUD->GetWorld()->GetTimeSeconds()*4.0f))/4.0f;
		}
	}
	return  FLinearColor( 
		FMath::Clamp(ResultTint.R, 0.0f, 1.0f ), 
		FMath::Clamp(ResultTint.G, 0.0f, 1.0f ),
		FMath::Clamp(ResultTint.B, 0.0f, 1.0f ),
		FMath::Clamp(ResultTint.A * GetCurrentOpacity(), 0.0f, 1.0f));
}

FReply SStrategyButtonWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsMouseButtonDown = true;
	return FReply::Handled();
}

FReply SStrategyButtonWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// we handle this message so that the game doesn't get mouse moves without mouse downs/ups
	if (bIsMouseButtonDown)
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


void SStrategyButtonWidget::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnMouseEnterDel.ExecuteIfBound(MyGeometry,MouseEvent);
}

void SStrategyButtonWidget::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bIsMouseButtonDown = false;
	OnMouseLeaveDel.ExecuteIfBound(MouseEvent);
}


FCursorReply SStrategyButtonWidget::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const
{
	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SStrategyButtonWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsMouseButtonDown)
	{
		bIsMouseButtonDown = false;
		if (bIsActionAllowed)
		{
			//execute our "OnClicked" delegate, if we have one
			if(OnClicked.IsBound() == true)
			{
				return OnClicked.Execute();
			}
		}
		else
		{
			//execute our "OnClickedDisabled" delegate, if we have one
			if(OnClickedDisabled.IsBound() == true)
			{
				return OnClickedDisabled.Execute();
			}
		}
	}
	return FReply::Handled();
}

FReply SStrategyButtonWidget::OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	// consume this event, so PlayerInput does not get it
	return FReply::Handled();
}

void SStrategyButtonWidget::DeferredShow()
{
	if (WidgetAnimation.IsAtStart() && !WidgetAnimation.IsPlaying())
	{
		WidgetAnimation.Play(this->AsShared());
	}
	else if (WidgetAnimation.IsInReverse())
	{
		WidgetAnimation.Reverse();
	}
}

void SStrategyButtonWidget::DeferredHide(bool bInstant)
{
	if (bInstant)
	{
		WidgetAnimation.JumpToStart();
	}  
	else
	{
		if (WidgetAnimation.IsAtEnd() && !WidgetAnimation.IsPlaying())
		{
			WidgetAnimation.PlayReverse(this->AsShared());
		} 
		else if (WidgetAnimation.IsForward())
		{
			WidgetAnimation.Reverse();
		} 
	}
}

bool SStrategyButtonWidget::IsAnimating() const
{
	return WidgetAnimation.IsPlaying();
}