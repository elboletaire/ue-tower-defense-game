// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"

#include "SStrategyButtonWidget.h"
#include "SStrategySlateHUDWidget.h"
#include "StrategyHelpers.h"
#include "StrategyAIController.h"
#include "StrategyBuilding.h"
#include "StrategyBuilding_Brewery.h"

AStrategyHUD::AStrategyHUD(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> BarFillObj(TEXT("/Game/UI/HUD/BarFill"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlayerTeamHPObj(TEXT("/Game/UI/HUD/PlayerTeamHealthBar"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> EnemyTeamHPObj(TEXT("/Game/UI/HUD/EnemyTeamHealthBar"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultActionObj(TEXT("/Game/UI/HUD/Actions/DefaultAction"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultCenterActionObj(TEXT("/Game/UI/HUD/Actions/DefaultActionBig"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ActionPauseObj(TEXT("/Game/UI/HUD/Actions/ActionPause"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> MenuButtonObj(TEXT("/Game/UI/MainMenu/MenuButton"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ResourceObj(TEXT("/Game/UI/HUD/Coin"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> LivesObj(TEXT("/Game/UI/HUD/Actions/Barrel"));
	

	static ConstructorHelpers::FObjectFinder<UMaterial> HUDMousePointerNeutralObj(TEXT("/Game/UI/Pointers/Neutral"));
	static ConstructorHelpers::FObjectFinder<UMaterial> HUDMousePointerAttackObj(TEXT("/Game/UI/Pointers/Enemy"));

	BarFillTexture = BarFillObj.Object;
	PlayerTeamHPTexture = PlayerTeamHPObj.Object;
	EnemyTeamHPTexture = EnemyTeamHPObj.Object;
	LivesTexture = LivesObj.Object;

	DefaultActionTexture = DefaultActionObj.Object;
	DefaultCenterActionTexture = DefaultCenterActionObj.Object;
	ActionPauseTexture = ActionPauseObj.Object;
	MenuButtonTexture = MenuButtonObj.Object;
	ResourceTexture =  ResourceObj.Object;

	MousePointerNeutral = HUDMousePointerNeutralObj.Object;
	MousePointerAttack = HUDMousePointerAttackObj.Object;

	MiniMapMargin = 40;
	bBlackScreenActive = false;
}


/**
 * This is the main drawing pump.  It will determine which hud we need to draw (Game or PostGame).  Any drawing that should occur
 * regardless of the game state should go here.
 */
void AStrategyHUD::DrawHUD()
{
	if (bBlackScreenActive)
	{
		FCanvasTileItem TileItem( FVector2D( 0.0f, 0.0f ), FVector2D( Canvas->ClipX,Canvas->ClipY ), FLinearColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem( TileItem );
		return;
	}

	if ( GEngine && GEngine->GameViewport )
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		UIScale = ViewportSize.X / 2048.0f;
	}

	Super::DrawHUD();
	bool bConsoleOpen = false;

	AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState)
	{
		//Builds the widgets if they are not yet built
		BuildMenuWidgets();

		if (MyGameState->IsGameActive())
		{
			DrawActorsHealth();
		}
		DrawMiniMap();
		DrawLives();

		if (IsPauseMenuUp())
		{
			/*FCanvasTileItem TileItem( FVector2D( 0.0f, 0.0f ), FVector2D( Canvas->ClipX,Canvas->ClipY ), FLinearColor( 0.0f, 0.0f, 0.0f, 0.25f ) );
			TileItem.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem( TileItem );*/
		}

		if (SelectedActor.IsValid())
		{
			ActionGridPos = FVector2D(Canvas->Project(SelectedActor->GetActorLocation())) / UIScale - (MyHUDMenuWidget->ActionButtonsWidget->GetDesiredSize())/2;
		}
	}
}

void AStrategyHUD::ShowBlackScreen()
{
	HideAllActionButtons(true);
	bBlackScreenActive = true;
	GEngine->GameViewport->RemoveAllViewportWidgets();
}

void AStrategyHUD::DrawActorsHealth()
{
	for (AStrategyChar* TestChar : TActorRange<AStrategyChar>(GetWorld()))
	{
		if (TestChar->GetHealth() > 0)
		{
			AStrategyAIController* AIController = Cast<AStrategyAIController>(TestChar->Controller);
			if (AIController != NULL && AIController->IsLogicEnabled())
			{
				DrawHealthBar(TestChar,TestChar->GetHealth()/(float)TestChar->GetMaxHealth(),18*UIScale);
			}
		}
	}
	AStrategyGameState* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState)
	{
		// 0 - unknown/neutral team, two teams in total
		for (int8 Team = 1; Team < EStrategyTeam::MAX; Team++)
		{
			for (int32 i = 0; i < MyGameState->GetPlayerData(Team)->BuildingsList.Num(); i++) 
			{
				if (MyGameState->GetPlayerData(Team)->BuildingsList[i].IsValid())
				{
					AStrategyBuilding* const TestBuilding = Cast<AStrategyBuilding>(MyGameState->GetPlayerData(Team)->BuildingsList[i].Get());
					if (TestBuilding != NULL && TestBuilding->GetHealth() > 0 && !TestBuilding->IsBuildFinished())
					{
						DrawHealthBar(TestBuilding, TestBuilding->GetHealth()/(float)TestBuilding->GetMaxHealth(), 30*UIScale);
					}
				}
			}
		}
	}

}

void AStrategyHUD::DrawMiniMap()
{
	const AStrategyPlayerController* const PC = Cast<AStrategyPlayerController>(PlayerOwner);
	AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();

	// @todo, clean this up
	if (PC && MyGameState && MyGameState->MiniMapCamera.IsValid())
	{
		UTexture* MiniMapTexture = Cast<UTexture>(MyGameState->MiniMapCamera->GetCaptureComponent2D()->TextureTarget);
		const float MapWidth = (MyGameState->MiniMapCamera->MiniMapWidth - MiniMapMargin) * UIScale;
		const float MapHeight = (MyGameState->MiniMapCamera->MiniMapHeight - MiniMapMargin) * UIScale;
		const FVector WorldCenter = MyGameState->WorldBounds.GetCenter();
		const FVector WorldExtent = MyGameState->WorldBounds.GetExtent();
		// Use a fixed yaw of 270.0f here instead of calculating (270.0f + MyGameState->MiniMapCamera->GetRootComponent()->GetComponentRotation().Roll).
		const FRotationMatrix RotationMatrix(FRotator(0.0f, 270.0f, 0.0f));
		const FVector2D Offset(MiniMapMargin * UIScale + (MapWidth/2.0f), Canvas->ClipY - (MapHeight/2.0f) - MiniMapMargin * UIScale );

		if (MiniMapTexture)
		{
			FCanvasTileItem MapTileItem( FVector2D( 0.0f, 0.0f), FVector2D( 0.0f, 0.0f ), FLinearColor::White );
			MapTileItem.Texture = MiniMapTexture->Resource;
			MapTileItem.Size = FVector2D( MapWidth, MapHeight );
			MapTileItem.BlendMode = SE_BLEND_Opaque;
			Canvas->DrawItem( MapTileItem, FVector2D( MiniMapMargin * UIScale, Canvas->ClipY - MapHeight - MiniMapMargin * UIScale ) );
		}
		FCanvasTileItem TileItem( FVector2D( 0.0f, 0.0f), FVector2D( 0.0f, 0.0f ), FLinearColor::White );
		TileItem.Size = FVector2D( 6 * UIScale, 6 * UIScale );
		for (AStrategyChar* TestChar : TActorRange<AStrategyChar>(GetWorld()))
		{
			if (TestChar->GetHealth() > 0 )
			{
				AStrategyAIController* AIController = Cast<AStrategyAIController>(TestChar->Controller);
				if (AIController != NULL && AIController->IsLogicEnabled())
				{
					FLinearColor DrawColor;
					if (PC != NULL && TestChar->GetTeamNum() == PC->GetTeamNum())
					{
						DrawColor = FColor( 49, 137, 253, 255);
					} 
					else
					{
						DrawColor = FColor( 242, 114, 16, 255);
					}
					const FVector CenterRelativeLocation = RotationMatrix.TransformPosition(TestChar->GetActorLocation() - WorldCenter);
					const FVector2D MiniMapPoint = FVector2D(CenterRelativeLocation.X / WorldExtent.X, CenterRelativeLocation.Y / WorldExtent.Y);
					TileItem.SetColor( DrawColor );
					Canvas->DrawItem( TileItem, FVector2D( Offset.X + MiniMapPoint.X * (MapWidth/2.0f), Offset.Y + MiniMapPoint.Y * (MapHeight/2.0f) ) );
				}
			}
		}


		ULocalPlayer* MyPlayer =  Cast<ULocalPlayer>(PC->Player);
		FVector2D ScreenCorners[4] = { FVector2D(0, 0), FVector2D(Canvas->ClipX, 0), FVector2D(Canvas->ClipX, Canvas->ClipY), FVector2D(0, Canvas->ClipY) };
		const FPlane GroundPlane = FPlane(FVector(0, 0, MyGameState->WorldBounds.Max.Z), FVector::UpVector);
		for (int32 i = 0; i < 4; i++)
		{
			FVector RayOrigin, RayDirection;
			FStrategyHelpers::DeprojectScreenToWorld(ScreenCorners[i], MyPlayer, RayOrigin, RayDirection);
			const FVector GroundPoint = FStrategyHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);
			const FVector CenterRelativeLocation = RotationMatrix.TransformPosition(GroundPoint - WorldCenter);
			MiniMapPoints[i] = FVector2D(CenterRelativeLocation.X / WorldExtent.X, CenterRelativeLocation.Y / WorldExtent.Y);
		}
	} 
}

void AStrategyHUD::BuildMenuWidgets()
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	if (!MyHUDMenuWidget.IsValid())
	{
		const AStrategyPlayerController* PCOwner = Cast<AStrategyPlayerController>(PlayerOwner);

		if (PCOwner)
		{
			SAssignNew(MyHUDMenuWidget, SStrategySlateHUDWidget)
			.OwnerHUD(this);

			if (MyHUDMenuWidget.IsValid())
			{

				GEngine->GameViewport->AddViewportWidgetContent(
					SNew(SWeakWidget)
					.PossiblyNullContent(MyHUDMenuWidget.ToSharedRef())
				);

				MyHUDMenuWidget->ActionButtonsWidget->SetVisibility(EVisibility::Visible);
				MyHUDMenuWidget->ActionWidgetPosition.BindUObject(this,&AStrategyHUD::GetActionsWidgetPos);

				if (ActionPauseTexture != NULL)
				{
					MyHUDMenuWidget->PauseButton->SetImage(ActionPauseTexture);
					MyHUDMenuWidget->PauseButton->DeferredShow();
				}
				if (MenuButtonTexture != NULL)
				{
					for (uint8 i = 0; i < MyHUDMenuWidget->PauseMenuButtons.Num(); i++)
					{
						MyHUDMenuWidget->PauseMenuButtons[i]->SetImage(MenuButtonTexture);
					}
				}
			}
		}
	}
}


FVector2D AStrategyHUD::GetActionsWidgetPos() const
{
	return ActionGridPos;
}

TSharedPtr<FActionButtonInfo> AStrategyHUD::GetActionButton(int32 Index) const
{
	if (MyHUDMenuWidget.IsValid() &&
		MyHUDMenuWidget->ActionButtonsWidget.IsValid() &&
		MyHUDMenuWidget->ActionButtonsWidget->ActionButtons.IsValidIndex(Index))
	{
		return MyHUDMenuWidget->ActionButtonsWidget->ActionButtons[Index];
	}

	return TSharedPtr<FActionButtonInfo>();
}

TSharedPtr<SStrategySlateHUDWidget> AStrategyHUD::GetHUDWidget() const
{
	return MyHUDMenuWidget;
}


void AStrategyHUD::ClearActionRequiredStates()
{
	if (MyHUDMenuWidget.IsValid() &&
		MyHUDMenuWidget->ActionButtonsWidget.IsValid())
	{
		for (int32 i = MyHUDMenuWidget->ActionButtonsWidget->ActionButtons.Num() - 1; i >= 0; i--)
		{
			MyHUDMenuWidget->ActionButtonsWidget->ActionButtons[i]->Widget->SetUserActionRequired(false);
		}
	}
}

void AStrategyHUD::HideAllActionButtons(bool bInstantHide)
{
	if (MyHUDMenuWidget.IsValid() &&
		MyHUDMenuWidget->ActionButtonsWidget.IsValid())
	{
		for (int32 i = MyHUDMenuWidget->ActionButtonsWidget->ActionButtons.Num() - 1; i >= 0; i--)
		{
			MyHUDMenuWidget->ActionButtonsWidget->ActionButtons[i]->Widget->DeferredHide(bInstantHide);
			MyHUDMenuWidget->ActionButtonsWidget->ActionButtons[i]->Data.Visibility = EVisibility::Hidden;
		}
	}
}

void AStrategyHUD::DrawLives() const
{
	AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	AStrategyBuilding_Brewery const* const Brewery = MyGameState ? MyGameState->GetPlayerData(EStrategyTeam::Player)->Brewery.Get() : NULL;

	uint8 const Lives = Brewery ? Brewery->GetNumberOfLives() : 0;

	float const TextureDrawWidth = LivesTexture->GetSurfaceWidth() * UIScale;
	float const TextureDrawHeight =  LivesTexture->GetSurfaceHeight() * UIScale;

	FCanvasTileItem TileItem(FVector2D(0,0), LivesTexture->Resource, 
							 FVector2D(TextureDrawWidth, TextureDrawHeight), FLinearColor::White );
	TileItem.BlendMode = SE_BLEND_Translucent;
	for (int32 i=0; i < Lives; i++)
	{
		TileItem.Position = FVector2D(Canvas->ClipX - Lives*TextureDrawWidth + i*TextureDrawWidth, 0);
		Canvas->DrawItem(TileItem);
	}
}

void AStrategyHUD::DrawHealthBar(AActor* ForActor, float HealthPercentage, int32 BarHeight, int32 OffsetY) const
{
	FBox BB = ForActor->GetComponentsBoundingBox();
	FVector Center = BB.GetCenter();
	FVector Extent = BB.GetExtent();
	FVector2D Center2D = FVector2D(Canvas->Project(FVector(Center.X,Center.Y,Center.Z + Extent.Z)));
	float ActorExtent = 40;
	if (Cast<APawn>(ForActor) != NULL)
	{
		AStrategyChar* StrategyChar = Cast<AStrategyChar>(ForActor);
		if( ( StrategyChar != NULL ) && ( StrategyChar->GetCapsuleComponent() != NULL ) )
		{
			ActorExtent = StrategyChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
		}
	}
	else if (Cast<AStrategyBuilding>(ForActor) != NULL)
	{
		Center2D = FVector2D(Canvas->Project(ForActor->GetActorLocation()));
		ActorExtent = 60;
	}

	FVector Pos1 = Canvas->Project(FVector(Center.X,Center.Y - ActorExtent*2, Center.Z + Extent.Z));
	FVector Pos2 = Canvas->Project(FVector(Center.X,Center.Y + ActorExtent*2, Center.Z + Extent.Z));
	float HealthBarLength = (Pos2-Pos1).Size2D();

	AStrategyPlayerController* MyPC = GetPlayerController();
	IStrategyTeamInterface* ActorTeam = Cast<IStrategyTeamInterface>(ForActor);
	UTexture2D* HealthBarTexture = EnemyTeamHPTexture;

	if (ActorTeam != NULL && MyPC != NULL && ActorTeam->GetTeamNum() == MyPC->GetTeamNum())
	{
		HealthBarTexture = PlayerTeamHPTexture;
	} 
	float X = Center2D.X - HealthBarLength/2;
	float Y = Center2D.Y + OffsetY;
	FCanvasTileItem TileItem( FVector2D( X, Y ), HealthBarTexture->Resource, FVector2D( HealthBarLength * HealthPercentage,  BarHeight ), FLinearColor::White );
	TileItem.BlendMode = SE_BLEND_Translucent;
	TileItem.UV1 = FVector2D(HealthPercentage, 1.0f);

	Canvas->DrawItem( TileItem );
	//Fill the rest of health with gray gradient texture
	X = Center2D.X-HealthBarLength/2 + HealthBarLength * HealthPercentage;
	Y = Center2D.Y + OffsetY;
	TileItem.Position = FVector2D( X, Y );
	TileItem.Texture = BarFillTexture->Resource;
	TileItem.UV1 = FVector2D(1.0f, 1.0f);
	TileItem.Size = FVector2D( HealthBarLength * (1.0f - HealthPercentage), BarHeight );
	TileItem.SetColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));
	Canvas->DrawItem( TileItem );	
}



void AStrategyHUD::DrawMousePointer()
{
#if PLATFORM_DESKTOP
	if (PlayerOwner)
	{
		FVector2D MousePosition;
		PlayerOwner->GetMousePosition(MousePosition.X, MousePosition.Y);

		const FVector2D PointerSize(60.0f, 30.0f);
		const FVector2D DrawPos(MousePosition.X - PointerSize.X * 0.5f, MousePosition.Y - PointerSize.Y * 0.5f);

		DrawMaterialSimple(MousePointerNeutral, DrawPos.X, DrawPos.Y, PointerSize.X, PointerSize.Y);
	}
#endif
}

bool AStrategyHUD::IsPauseMenuUp() const
{
	if (MyHUDMenuWidget.IsValid())
	{
		return MyHUDMenuWidget->IsPauseMenuUp();
	}
	else 
	{
		return false;
	}
}

void AStrategyHUD::TogglePauseMenu()
{
	if (MyHUDMenuWidget.IsValid())
	{
		MyHUDMenuWidget->TogglePauseMenu();
	}
}

AStrategyPlayerController* AStrategyHUD::GetPlayerController() const
{
	return Cast<AStrategyPlayerController>(PlayerOwner);
}

void AStrategyHUD::SetActionGridActor(AActor* InSelectedActor)
{
	SelectedActor.Reset();
	SelectedActor = MakeWeakObjectPtr(InSelectedActor);
}
