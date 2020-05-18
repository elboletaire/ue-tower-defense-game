// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SStrategyActionGrid.h"
#include "SStrategyButtonWidget.h"


void SStrategyActionGrid::Construct(const FArguments& InArgs)
{
	const int32 PaddingValue = 2;
	const int32 GridRows = 3;
	const int32 GridCols = 3;

	OwnerHUD = InArgs._OwnerHUD;
	ActionButtons.Reset();
	for (uint8 i=0; i < GridRows*GridCols; i++)
	{
		ActionButtons.Add(MakeShareable(new FActionButtonInfo()));
	}

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SAssignNew(ActionGrid, SGridPanel)
		]
	];

	for (int32 i = 0; i < GridRows; i++)
	{
		for (int32 j=0; j < GridCols; j++)
		{
			ActionGrid->AddSlot(j,i)
			.Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this, &SStrategyActionGrid::GetActionPadding, i*GridRows+j)))
			[
				SAssignNew(ActionButtons[i*GridRows+j]->Widget,SStrategyButtonWidget)
				.OwnerHUD(OwnerHUD)
				.ToolTip(SNew(SToolTip).Text(this, &SStrategyActionGrid::GetTooltip,i*GridRows+j))
				.IsEnabled(this, &SStrategyActionGrid::GetEnabled, i*GridRows+j)
				.Visibility(this, &SStrategyActionGrid::GetActionVisibility, i*GridRows+j)
				.OnClicked(this, &SStrategyActionGrid::TriggerAction, i*GridRows+j)
				.ButtonText(this, &SStrategyActionGrid::GetActionCostText, i*GridRows+j)
				.CenterText(this, &SStrategyActionGrid::GetActionText, i*GridRows+j)
				.CornerText(this,&SStrategyActionGrid::GetActionQueueText, i*GridRows+j)
				.CoinIconVisible(this, &SStrategyActionGrid::GetCoinIconVisibility, i*GridRows+j)
			];
		}
	}
}

FMargin SStrategyActionGrid::GetActionPadding(int32 idx) const
{
	const float SmallMargin = 6;
	const float LargeMargin = 60;
	//this is difference in size from big action icon(which is used only in the center) to small action icon divided by 2
	//example (86 - 64)/2 = 11
	const float CenterMarginOffset = 0;//11;
	//Margins are set up so buttons form a circle
	const FMargin ActionButtonsMargins[9] = { 
		FMargin(LargeMargin, LargeMargin, SmallMargin, SmallMargin), 
		FMargin(SmallMargin, SmallMargin, SmallMargin, LargeMargin - CenterMarginOffset),
		FMargin(SmallMargin, LargeMargin, LargeMargin, SmallMargin),

		FMargin(SmallMargin, SmallMargin, LargeMargin - CenterMarginOffset, SmallMargin),
		FMargin(SmallMargin),
		FMargin(LargeMargin - CenterMarginOffset, SmallMargin, SmallMargin, SmallMargin),

		FMargin(LargeMargin, SmallMargin, SmallMargin, LargeMargin),
		FMargin(SmallMargin, LargeMargin - CenterMarginOffset, SmallMargin, SmallMargin),
		FMargin(SmallMargin, SmallMargin, LargeMargin, LargeMargin)
	};

	return (idx >= 0 && idx <= 8) ? ActionButtonsMargins[idx] : FMargin(SmallMargin);
}

FReply SStrategyActionGrid::TriggerAction(int32 idx) const
{
	if (ActionButtons[idx]->Widget.IsValid() && 
		ActionButtons[idx]->Data.TriggerDelegate.IsBound() && 
		ActionButtons[idx]->Data.TriggerDelegate.Execute())
	{
		ActionButtons[idx]->Data.bIsEnabled = false;
		ActionButtons[idx]->Widget->DeferredHide();
	}
	return FReply::Handled();
}

FText SStrategyActionGrid::GetActionCostText(int32 idx) const
{
	return ActionButtons[idx]->Data.ActionCost != 0 ? FText::AsNumber(ActionButtons[idx]->Data.ActionCost) : FText::GetEmpty();
}

FText SStrategyActionGrid::GetActionText(int32 idx) const
{
	return ActionButtons[idx]->Data.StrButtonText;
}

FText SStrategyActionGrid::GetActionQueueText(int32 idx) const
{
	FText Result = FText::GetEmpty();
	if (ActionButtons[idx]->Data.GetQueueLengthDelegate.IsBound())
	{
		Result = ActionButtons[idx]->Data.GetQueueLengthDelegate.Execute();
	}
	return Result;
}

TOptional<EVisibility> SStrategyActionGrid::GetCoinIconVisibility(int32 idx) const
{
	return ActionButtons[idx]->Data.ActionCost == 0 ? EVisibility::Collapsed : EVisibility::Visible;
}

FText SStrategyActionGrid::GetTooltip(int32 idx) const
{
	return ActionButtons[idx]->Data.StrTooltip;
}

bool SStrategyActionGrid::GetEnabled(int32 idx) const
{
	const AStrategyPlayerController* const PC = Cast<AStrategyPlayerController>(OwnerHUD->PlayerOwner);
	AStrategyGameState* const MyGameState = OwnerHUD->GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState && PC)
	{
		FPlayerData* const PlayerData = MyGameState->GetPlayerData(PC->GetTeamNum());
		if (PlayerData)
		{
			if (PlayerData->ResourcesAvailable >= (uint32)FMath::Max(0, ActionButtons[idx]->Data.ActionCost))
			{
				ActionButtons[idx]->Widget->SetActionAllowed(ActionButtons[idx]->Data.bIsEnabled);
			} 
			else
			{
				ActionButtons[idx]->Widget->SetActionAllowed(false);
			}
		}
	}

	return true;
}

EVisibility SStrategyActionGrid::GetActionVisibility(int32 idx) const
{
	return ActionButtons[idx]->Widget->IsAnimating() ? EVisibility::Visible : ActionButtons[idx]->Data.Visibility;
}
