// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategyMenuWidget.h"
#include "Engine/Console.h"
#include "StrategyMenuHUD.h"
#include "StrategyMenuItem.h"
#include "SStrategyMenuItem.h"
#include "StrategyHUDSoundsWidgetStyle.h"
#include "StrategyMenuWidgetStyle.h"
#include "Runtime/Engine/Classes/Engine/Console.h"
#include "SDPIScaler.h"

#define LOCTEXT_NAMESPACE "SStrategyMenuWidget"

void SStrategyMenuWidget::Construct(const FArguments& InArgs)
{
	MenuStyle = &FStrategyStyle::Get().GetWidgetStyle<FStrategyMenuStyle>("DefaultStrategyMenuStyle");

	bMenuHiding = false;
	bControlsLocked = false;
	SelectedIndex = 0;
	MyMenuHUD = InArgs._MenuHUD;
	Visibility.Bind(this, &SStrategyMenuWidget::GetVisibility);
	UIScale.Bind(this, &SStrategyMenuWidget::GetUIScale);
	MenuPaddingX = 80.0f;
	MenuPaddingY = 30.0f;
	const float BgPadding = 20.0f;
	MenuOffsetX = 500.0f;
	MenuOffsetY = 200.0f;

	ChildSlot
	[
		SNew(SDPIScaler)
		.DPIScale(UIScale)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(&MenuStyle->BackgroundBrush)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(TAttribute<FMargin>(this,&SStrategyMenuWidget::GetMenuOffset))
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
					.DesiredSizeScale(this, &SStrategyMenuWidget::GetBottomScale)
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Left)
					.Clipping(EWidgetClipping::ClipToBounds)
					[
						SNew(SOverlay)
						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(BgPadding))
						[
							SNew(SImage)
							.Image(&MenuStyle->TileBackgroundBrush)
						]
						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->PopupBackgroundBrush)
							.ColorAndOpacity(this, &SStrategyMenuWidget::GetBottomColor)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							[
								SNew(SOverlay)
								+ SOverlay::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								.Padding(FMargin(MenuPaddingX,MenuPaddingY))
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									.Padding(TAttribute<FMargin>(this,&SStrategyMenuWidget::GetLeftMenuOffset))
									[
										SAssignNew(LeftBox, SVerticalBox)
									]
								]
								+ SOverlay::Slot()
									.HAlign(HAlign_Left)
									.VAlign(VAlign_Center)
									[
										SNew(SImage)
										.Image(&MenuStyle->LeftDecoration)
									]
								+ SOverlay::Slot()
									.HAlign(HAlign_Right)
									.VAlign(VAlign_Center)
									[
										SNew(SImage)
										.Image(&MenuStyle->RightDecoration)
									]
							]
						]
					]
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SCanvas)
				.Visibility(EVisibility::HitTestInvisible)
				+SCanvas::Slot()
				.Position(TAttribute<FVector2D>(this, &SStrategyMenuWidget::GetTopDecorPosition))
				.Size(TAttribute<FVector2D>(this, &SStrategyMenuWidget::GetTopDecorSize))
				[
					SAssignNew(TopDecorImage,SImage)
					.Image(&MenuStyle->TopDecoration)
				]
			]
		]
	];
	//Build left menu panel
	bLeftMenuChanging = false;
	bGoingBack = false;
	BuildLeftPanel(bGoingBack);

	//sets up whole main menu animations and launches them
	SetupAnimations();

	// Set up right side and launch animation if there is any submenu
	MyMenuHUD->NextMenu = (*MyMenuHUD->CurrentMenu)[SelectedIndex].SubMenu;
}

FVector2D SStrategyMenuWidget::GetTopDecorPosition() const
{
	FVector2D Result;

	FVector2D CurrentViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(CurrentViewportSize);
	}
	const float BoxWidth = LeftBox->GetDesiredSize().X;
	const float WidgetWidth = BoxWidth + MenuPaddingX * 2;
	const float WidgetHeight = LeftBox->GetDesiredSize().Y + MenuPaddingY * 2;
	const float Scale = UIScale.Get();
	const FVector2D& ImgSize = TopDecorImage->GetDesiredSize();
	const float SlideInAnimProgress = ButtonsPosXCurve.GetLerp();
	const float ScrolloutAnimProgress = LeftMenuScrollOutCurve.GetLerp();


	const float OffsetX = (CurrentViewportSize.X / Scale - (WidgetWidth - ScrolloutAnimProgress * BoxWidth + BoxWidth)) / 2.0f;
	const float Left = OffsetX + (CurrentViewportSize.X / Scale) - SlideInAnimProgress * (CurrentViewportSize.X / Scale);

	Result.X = Left + MenuOffsetX + (WidgetWidth - ImgSize.X) / 2.0f;
	Result.Y = MenuOffsetY - ImgSize.Y / 2.0f;

	return Result;
}

FVector2D SStrategyMenuWidget::GetTopDecorSize() const
{
	return FVector2D(TopDecorImage->GetDesiredSize());
}

float SStrategyMenuWidget::GetUIScale() const
{
	float Result = 0.5f;
	if ( GEngine && GEngine->GameViewport )
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		Result = ViewportSize.X / 2048.0f;
	}
	//as a fall back assume 1024x768
	return Result;
}


void SStrategyMenuWidget::HideMenu()
{
	if (!bMenuHiding)
	{
		if(MenuWidgetAnimation.IsAtEnd())
		{
			MenuWidgetAnimation.PlayReverse(this->AsShared());
		}
		else
		{
			MenuWidgetAnimation.Reverse();
		}
		bMenuHiding = true;
	}
}

void SStrategyMenuWidget::SetupAnimations()
{
	const float StartDelay = 0.0f;
	const float SecondDelay = 0.3f;
	const float AnimDuration = 0.5f;
	const float MenuChangeDuration = 0.2f;
	MenuWidgetAnimation = FCurveSequence();
	LeftMenuWidgetAnimation = FCurveSequence();
	LeftMenuScrollOutCurve = LeftMenuWidgetAnimation.AddCurve(0,MenuChangeDuration,ECurveEaseFunction::QuadInOut);
	LeftMenuWidgetAnimation.Play(this->AsShared());

	//Define the fade in animation
	TopColorCurve = MenuWidgetAnimation.AddCurve(StartDelay, AnimDuration, ECurveEaseFunction::QuadInOut);

	//now, we want these to animate some time later

	//rolling out
	BottomScaleYCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	//fading in
	BottomColorCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	//moving from left side off screen
	ButtonsPosXCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);

	FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound);

}

void SStrategyMenuWidget::LockControls(bool bEnable)
{
	bControlsLocked = bEnable;
}

void SStrategyMenuWidget::BuildLeftPanel(bool bInGoingBack)
{
	LeftBox->ClearChildren();
	if (bInGoingBack && MyMenuHUD->MenuHistory.Num() > 0)
	{
		MyMenuHUD->CurrentMenu = MyMenuHUD->MenuHistory.Pop();
		if ((*MyMenuHUD->CurrentMenu).Num() > 0 && (*MyMenuHUD->CurrentMenu)[0].SubMenu.IsValid())
		{
			MyMenuHUD->NextMenu = (*MyMenuHUD->CurrentMenu)[0].SubMenu;
		}
	} 
	else if (PendingLeftMenu.IsValid())
	{
		MyMenuHUD->MenuHistory.Push(MyMenuHUD->CurrentMenu);
		MyMenuHUD->CurrentMenu = PendingLeftMenu;
	}

	//Setup the buttons
	for(int32 i = 0; i < (*MyMenuHUD->CurrentMenu).Num(); ++i)
	{
		//Add slots to the boxes...This is just to get around copy/pasting
		TSharedPtr<SStrategyMenuItem> TmpButton;
		TmpButton = SAssignNew((*MyMenuHUD->CurrentMenu)[i].Widget,SStrategyMenuItem)
			.OnClicked(this, &SStrategyMenuWidget::ButtonClicked, i)
			.Text((*MyMenuHUD->CurrentMenu)[i].Text );
		(*MyMenuHUD->CurrentMenu)[i].Widget->SetImage(MyMenuHUD->MenuButtonTexture);
		(*MyMenuHUD->CurrentMenu)[i].Widget->DeferredShow();
		LeftBox->AddSlot()
			.HAlign(HAlign_Left)
			.AutoHeight()
			.Padding(FMargin(0,10))
			[
				TmpButton.ToSharedRef()
			];
	}
	
	SelectedIndex = 0;
}


void SStrategyMenuWidget::EnterSubMenu()
{
	bLeftMenuChanging = true;
	bGoingBack = false;
	FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound);
}

void SStrategyMenuWidget::MenuGoBack()
{
	if (MyMenuHUD->MenuHistory.Num() > 0)
	{
		bLeftMenuChanging = true;
		bGoingBack = true;
		FSlateApplication::Get().PlaySound(MenuStyle->MenuExitSound);
	}
}

void SStrategyMenuWidget::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	//Always tick the super
	SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

	//ugly code seeing if the console is open
	UConsole* ViewportConsole = (GEngine->GameViewport != NULL) ? GEngine->GameViewport->ViewportConsole : NULL;
	if (ViewportConsole != NULL && (ViewportConsole->ConsoleState == "Typing" || ViewportConsole->ConsoleState == "Open"))
	{
		if (!bConsoleVisible)
		{
			bConsoleVisible = true;
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
		}
	}
	else
	{
		if (bConsoleVisible)
		{
			bConsoleVisible = false;
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		}
	}

	if (MenuWidgetAnimation.IsAtStart() && !bMenuHiding)
	{
		//Start the menu widget animation, set keyboard focus
		FadeIn();
	}
	if (MenuWidgetAnimation.IsAtEnd())
	{
		if (bLeftMenuChanging)
		{
			if (LeftMenuWidgetAnimation.IsAtEnd())
			{
				PendingLeftMenu = MyMenuHUD->NextMenu;
				if (MyMenuHUD->NextMenu.IsValid() && MyMenuHUD->NextMenu->Num() > 0
					&& ((*MyMenuHUD->NextMenu))[0].SubMenu.IsValid() && ((*MyMenuHUD->NextMenu))[0].SubMenu->Num() > 0)
				{
					MyMenuHUD->NextMenu = ((*MyMenuHUD->NextMenu))[0].SubMenu;
				} 
				else 
				{
					MyMenuHUD->NextMenu.Reset();
				}
				LeftMenuWidgetAnimation.PlayReverse(this->AsShared());
			}
			if (!LeftMenuWidgetAnimation.IsPlaying())
			{
				if (MyMenuHUD->CurrentMenu.IsValid() && MyMenuHUD->CurrentMenu->Num() > 0)
				{
					BuildLeftPanel(bGoingBack);
					LeftMenuWidgetAnimation.Play(this->AsShared());
				}
				bLeftMenuChanging = false;
			}
		}
	}
}

FMargin SStrategyMenuWidget::GetMenuOffset() const
{
	FMargin Result;
	FVector2D CurrentViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(CurrentViewportSize);
	}
	const float WidgetWidth = LeftBox->GetDesiredSize().X + MenuPaddingX*2;
	const float WidgetHeight = LeftBox->GetDesiredSize().Y + MenuPaddingY*2;
	const float Scale = UIScale.Get();
	const float OffsetX = (CurrentViewportSize.X / Scale - WidgetWidth) / 2.0f + MenuOffsetX;
	const float AnimProgress = ButtonsPosXCurve.GetLerp();
	Result = FMargin(OffsetX + (CurrentViewportSize.X / Scale) - AnimProgress * (CurrentViewportSize.X / Scale), MenuOffsetY, 0, 0);
	return Result;
}

FMargin SStrategyMenuWidget::GetLeftMenuOffset() const
{
	const float ScrolledWidth = LeftBox->GetDesiredSize().X;
	return FMargin(0, 0,-ScrolledWidth+LeftMenuScrollOutCurve.GetLerp()*ScrolledWidth,0);
}

FVector2D SStrategyMenuWidget::GetBottomScale() const
{
	return FVector2D(BottomScaleYCurve.GetLerp(), BottomScaleYCurve.GetLerp());
}

FLinearColor SStrategyMenuWidget::GetBottomColor() const
{
	return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), BottomColorCurve.GetLerp());
}

FLinearColor SStrategyMenuWidget::GetTopColor() const
{
	return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), TopColorCurve.GetLerp());
}

FReply SStrategyMenuWidget::ButtonClicked(int32 ButtonIndex)
{
	if (bControlsLocked)
	{
		return FReply::Handled();
	}

	if (SelectedIndex != ButtonIndex)
	{
		SelectedIndex = ButtonIndex;
		MyMenuHUD->NextMenu = (*MyMenuHUD->CurrentMenu)[SelectedIndex].SubMenu;

		const FStrategyHUDSoundsStyle& HUDSounds = FStrategyStyle::Get().GetWidgetStyle<FStrategyHUDSoundsStyle>("DefaultStrategyHUDSoundsStyle");
		FSlateApplication::Get().PlaySound(HUDSounds.MenuItemChangeSound);
	}

	TSharedPtr<TArray<class FStrategyMenuItem>> CurrentMenu = MyMenuHUD.Get()->CurrentMenu;
	if ((*CurrentMenu)[SelectedIndex].OnConfirmMenuItem.IsBound())
	{
		(*CurrentMenu)[SelectedIndex].OnConfirmMenuItem.Execute();
	} 
	else if ((*CurrentMenu)[SelectedIndex].SubMenu.IsValid() && (*CurrentMenu)[SelectedIndex].SubMenu->Num() > 0)
	{
		EnterSubMenu();
	}
	return FReply::Unhandled();
}

void SStrategyMenuWidget::FadeIn()
{
	//Start the menu widget playing
	MenuWidgetAnimation.Play(this->AsShared());

	//Go into UI mode
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

FReply SStrategyMenuWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//If we clicked anywhere, jump to the end
	if(MenuWidgetAnimation.IsPlaying())
	{
		MenuWidgetAnimation.JumpToEnd();
	}

	return FReply::Unhandled();
}

FReply SStrategyMenuWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().LockMouseToWidget(SharedThis(this));
}

EVisibility SStrategyMenuWidget::GetVisibility() const
{
	return bConsoleVisible ? EVisibility::HitTestInvisible : EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
