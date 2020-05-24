// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyHelpers.h"
#include "SStrategyButtonWidget.h"
#include "StrategyCheatManager.h"
#include "StrategyCameraComponent.h"
#include "StrategyInput.h"
#include "StrategySpectatorPawn.h"
#include "StrategySelectionInterface.h"
#include "StrategyInputInterface.h"


AStrategyPlayerController::AStrategyPlayerController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, bIgnoreInput(false)
{
	CheatClass = UStrategyCheatManager::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	SetHidden(false);
	bShowMouseCursor = true;
}

void AStrategyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputHandler = NewObject<UStrategyInput>(this);

	BIND_1P_ACTION(InputHandler, EGameKey::Tap, IE_Pressed, &AStrategyPlayerController::OnTapPressed);
	BIND_1P_ACTION(InputHandler, EGameKey::Hold, IE_Pressed, &AStrategyPlayerController::OnHoldPressed);
	BIND_1P_ACTION(InputHandler, EGameKey::Hold, IE_Released, &AStrategyPlayerController::OnHoldReleased);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Pressed, &AStrategyPlayerController::OnSwipeStarted);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Repeat, &AStrategyPlayerController::OnSwipeUpdate);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Released, &AStrategyPlayerController::OnSwipeReleased);
	BIND_2P_ACTION(InputHandler, EGameKey::SwipeTwoPoints, IE_Pressed, &AStrategyPlayerController::OnSwipeTwoPointsStarted);
	BIND_2P_ACTION(InputHandler, EGameKey::SwipeTwoPoints, IE_Repeat, &AStrategyPlayerController::OnSwipeTwoPointsUpdate);
	BIND_2P_ACTION(InputHandler, EGameKey::Pinch, IE_Pressed, &AStrategyPlayerController::OnPinchStarted);
	BIND_2P_ACTION(InputHandler, EGameKey::Pinch, IE_Repeat, &AStrategyPlayerController::OnPinchUpdate);

	FInputActionBinding& ToggleInGameMenuBinding = InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AStrategyPlayerController::OnToggleInGameMenu);
	ToggleInGameMenuBinding.bExecuteWhenPaused = true;

}

void AStrategyPlayerController::GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir)
{
	Super::GetAudioListenerPosition(OutLocation, OutFrontDir, OutRightDir);

	AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportFrame && MyGameState != NULL && MyGameState->MiniMapCamera.IsValid())
	{
		// Set Listener position to be the center of the viewport, projected into the game world.

		FViewport* const Viewport = GEngine->GameViewport->ViewportFrame->GetViewport();
		if (Viewport)
		{
			FVector2D const ScreenRes = Viewport->GetSizeXY();

			float GroundLevel = MyGameState->MiniMapCamera->AudioListenerGroundLevel;
			const FPlane GroundPlane = FPlane(FVector(0,0,GroundLevel), FVector::UpVector);
			ULocalPlayer* const MyPlayer = Cast<ULocalPlayer>(Player);

			// @todo: once PlayerCamera is back in, we can just get the ray origin and dir from that instead of 
			// needing to deproject. will be much simpler.
			FVector RayOrigin, RayDirection;
			FVector2D const ScreenCenterPoint = ScreenRes * 0.5f;
			FStrategyHelpers::DeprojectScreenToWorld(ScreenCenterPoint, MyPlayer, RayOrigin, RayDirection);

			FVector const WorldPoint = FStrategyHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);
			FVector const AudioListenerOffset = MyGameState->MiniMapCamera->AudioListenerLocationOffset;
			OutLocation = WorldPoint.GetClampedToSize(MyGameState->WorldBounds.Min.GetMin(), MyGameState->WorldBounds.Max.GetMax()) + AudioListenerOffset;

			bool bUseCustomOrientation = MyGameState->MiniMapCamera->bUseAudioListenerOrientation;
			if (bUseCustomOrientation)
			{
				OutFrontDir = MyGameState->MiniMapCamera->AudioListenerFrontDir;
				OutRightDir = MyGameState->MiniMapCamera->AudioListenerRightDir;
			}
		}
	}
}

void AStrategyPlayerController::OnToggleInGameMenu()
{
	AStrategyHUD* const StrategyHUD = Cast<AStrategyHUD>(GetHUD());
	if (StrategyHUD)
	{
		StrategyHUD->TogglePauseMenu();
	}
}

void AStrategyPlayerController::UpdateRotation(float DeltaTime)
{
	FRotator ViewRotation(0, 0, 0);
	FRotator DeltaRot(0, 0, 0);

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}
	
	SetControlRotation(ViewRotation);
}

void AStrategyPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	if (!bGamePaused && PlayerInput && InputHandler && !bIgnoreInput)
	{
		InputHandler->UpdateDetection(DeltaTime);
	}

	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
		
	if (!bIgnoreInput )
	{
		const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		AStrategySpectatorPawn* StrategyPawn = GetStrategySpectatorPawn();		
		if(( StrategyPawn != NULL ) && ( LocalPlayer != NULL ))
		{
			// Create the bounds for the minimap so we can add it as a 'no scroll' zone.
			AStrategyHUD* const HUD = Cast<AStrategyHUD>(GetHUD());
			AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
			if( (MyGameState != NULL ) && ( MyGameState->MiniMapCamera.IsValid() == true ) )
			{
				if( LocalPlayer->ViewportClient != NULL )
				{
					const FIntPoint ViewportSize = LocalPlayer->ViewportClient->Viewport->GetSizeXY();
					const uint32 ViewTop = FMath::TruncToInt(LocalPlayer->Origin.Y * ViewportSize.Y);
					const uint32 ViewBottom = ViewTop + FMath::TruncToInt(LocalPlayer->Size.Y * ViewportSize.Y);

					FVector TopLeft( HUD->MiniMapMargin, ViewBottom - HUD->MiniMapMargin - MyGameState->MiniMapCamera->MiniMapHeight, 0 );
					FVector BottomRight( (int32)MyGameState->MiniMapCamera->MiniMapWidth, MyGameState->MiniMapCamera->MiniMapHeight, 0 );
					FBox MiniMapBounds( TopLeft, TopLeft + BottomRight );
					StrategyPawn->GetStrategyCameraComponent()->AddNoScrollZone( MiniMapBounds );
					StrategyPawn->GetStrategyCameraComponent()->UpdateCameraMovement( this );
				}
			}
		}		
	}
}

void AStrategyPlayerController::SetCameraTarget(const FVector& CameraTarget)
{	
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->SetCameraTarget(CameraTarget);
	}
}

AActor* AStrategyPlayerController::GetFriendlyTarget(const FVector2D& ScreenPoint, FVector& WorldPoint) const
{
	FHitResult Hit;
	if ( GetHitResultAtScreenPosition(ScreenPoint, COLLISION_WEAPON, true, Hit) )
	{
		if ( !AStrategyGameMode::OnEnemyTeam(Hit.GetActor(), this) )
		{
			WorldPoint = Hit.ImpactPoint;
			return Hit.GetActor();
		}
	}

	return NULL;
}

void AStrategyPlayerController::SetIgnoreInput(bool bIgnore)
{
	bIgnoreInput = bIgnore;
}

uint8 AStrategyPlayerController::GetTeamNum() const
{
	return EStrategyTeam::Player;
};

void AStrategyPlayerController::SetSelectedActor(AActor* NewSelectedActor, const FVector& NewPosition)
{
	if (SelectedActor != NewSelectedActor)
	{
		// attempt to unselect current selection
		AActor* const OldSelection = SelectedActor.Get();
		if ( OldSelection && OldSelection->GetClass()->ImplementsInterface(UStrategySelectionInterface::StaticClass()) )
		{
			if ( IStrategySelectionInterface::Execute_OnSelectionLost(OldSelection, NewPosition, NewSelectedActor) )
			{
				SelectedActor = NULL;
			}
		}
		
		if ( !SelectedActor.IsValid() )
		{
			// attempt to select new selection
			if (NewSelectedActor && NewSelectedActor->GetClass()->ImplementsInterface(UStrategySelectionInterface::StaticClass()) )
			{
				if ( IStrategySelectionInterface::Execute_OnSelectionGained(NewSelectedActor) )
				{
					SelectedActor = NewSelectedActor;
				}
			}
		}
	}
}

void AStrategyPlayerController::OnTapPressed(const FVector2D& ScreenPosition, float DownTime)
{
	FVector WorldPosition(0.f);
	AActor* const HitActor = GetFriendlyTarget(ScreenPosition, WorldPosition);
	
	SetSelectedActor(HitActor, WorldPosition);

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UStrategyInputInterface::StaticClass()) )
	{
		IStrategyInputInterface::Execute_OnInputTap(HitActor);
	}
}

void AStrategyPlayerController::OnHoldPressed(const FVector2D& ScreenPosition, float DownTime)
{
	FVector WorldPosition(0.0f);
	AActor* const HitActor = GetFriendlyTarget(ScreenPosition, WorldPosition);

	SetSelectedActor(HitActor, WorldPosition);

	if ( HitActor && HitActor->GetClass()->ImplementsInterface(UStrategyInputInterface::StaticClass()) )
	{
		IStrategyInputInterface::Execute_OnInputHold(HitActor);
	}
}

void AStrategyPlayerController::OnHoldReleased(const FVector2D& ScreenPosition, float DownTime)
{
	AActor* const Selected = SelectedActor.Get();
	if ( Selected && Selected->GetClass()->ImplementsInterface(UStrategyInputInterface::StaticClass()) )
	{
		IStrategyInputInterface::Execute_OnInputHoldReleased(Selected, DownTime);
	}
}

void AStrategyPlayerController::OnSwipeStarted(const FVector2D& AnchorPosition, float DownTime)
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->OnSwipeStarted(AnchorPosition);
	}

	FVector WorldPosition(0.0f);
	AActor* const HitActor = GetFriendlyTarget(AnchorPosition, WorldPosition);

	SetSelectedActor(HitActor, WorldPosition);

	/** Get our position in 3d space */
	if (SelectedActor.IsValid())
	{
		SwipeAnchor3D = SelectedActor->GetActorLocation();
	}

	PrevSwipeScreenPosition = AnchorPosition;
}

void AStrategyPlayerController::OnSwipeUpdate(const FVector2D& ScreenPosition, float DownTime)
{
	AActor* const Selected = SelectedActor.Get();
	if ( Selected && Selected->GetClass()->ImplementsInterface(UStrategyInputInterface::StaticClass()) )
	{
		ULocalPlayer* const MyPlayer =  Cast<ULocalPlayer>(Player);
		const FPlane GroundPlane = FPlane(FVector(0, 0, SelectedActor->GetActorLocation().Z), FVector(0,0,1));

		FVector RayOrigin, RayDirection;
		FStrategyHelpers::DeprojectScreenToWorld(ScreenPosition, MyPlayer, RayOrigin, RayDirection);
		const FVector ScreenPosition3D = FStrategyHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);

		IStrategyInputInterface::Execute_OnInputSwipeUpdate(Selected, ScreenPosition3D - SwipeAnchor3D);
	}
	else
	{
		if (GetCameraComponent() != NULL)
		{
			GetCameraComponent()->OnSwipeUpdate(ScreenPosition);
		}
	}

	PrevSwipeScreenPosition = ScreenPosition;
}

void AStrategyPlayerController::OnSwipeReleased(const FVector2D& ScreenPosition, float DownTime)
{
	AActor* const Selected = SelectedActor.Get();
	if ( Selected && Selected->GetClass()->ImplementsInterface(UStrategyInputInterface::StaticClass()) )
	{
		ULocalPlayer* const MyPlayer =  Cast<ULocalPlayer>(this->Player);
		const FPlane GroundPlane = FPlane(FVector(0, 0, SelectedActor->GetActorLocation().Z), FVector(0,0,1));

		FVector RayOrigin, RayDirection;
		FStrategyHelpers::DeprojectScreenToWorld(ScreenPosition, MyPlayer, RayOrigin, RayDirection);
		const FVector ScreenPosition3D = FStrategyHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);

		IStrategyInputInterface::Execute_OnInputSwipeReleased(Selected, ScreenPosition3D - SwipeAnchor3D, DownTime);
	}
	else
	{
		if (GetCameraComponent() != NULL)
		{
			GetCameraComponent()->OnSwipeReleased(ScreenPosition);
		}
	}
}

void AStrategyPlayerController::OnSwipeTwoPointsStarted(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	PrevSwipeMidPoint = (ScreenPosition1 + ScreenPosition2) * 0.5f;
}

void AStrategyPlayerController::OnSwipeTwoPointsUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	const FVector2D SwipeMidPoint = (ScreenPosition1 + ScreenPosition2) * 0.5f;
	const FVector MoveDir = FVector(SwipeMidPoint - PrevSwipeMidPoint, 0.0f).GetSafeNormal();
	const float SwipeSpeed = 10000.0f;

	const FRotationMatrix R(PlayerCameraManager->GetCameraRotation() + FRotator(0.0, 90.0, 0.0));
	const FVector WorldSpaceAccel = R.TransformVector(MoveDir) * SwipeSpeed;
	if (GetSpectatorPawn())
	{
		GetSpectatorPawn()->AddMovementInput(WorldSpaceAccel, 1.f);
	}	

	PrevSwipeMidPoint = SwipeMidPoint;
}

void AStrategyPlayerController::OnPinchStarted(const FVector2D& AnchorPosition1, const FVector2D& AnchorPosition2, float DownTime)
{
	// Pass the pinch through to the camera component.
	if (GetCameraComponent() != NULL)
	{
		 GetCameraComponent() ->OnPinchStarted( AnchorPosition1, AnchorPosition2, DownTime );
	}
}

void AStrategyPlayerController::OnPinchUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	// Pass the pinch through to the camera component.
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->OnPinchUpdate(InputHandler, ScreenPosition1, ScreenPosition2, DownTime);
	}
}

AStrategySpectatorPawn* AStrategyPlayerController::GetStrategySpectatorPawn() const
{
	return Cast<AStrategySpectatorPawn>( GetSpectatorPawn() );
}

UStrategyCameraComponent* AStrategyPlayerController::GetCameraComponent() const
{
	UStrategyCameraComponent* CameraComponent = NULL;
	if (GetStrategySpectatorPawn() != NULL)
	{
		CameraComponent = GetStrategySpectatorPawn()->GetStrategyCameraComponent();
	}
	return CameraComponent;
}

void AStrategyPlayerController::MouseLeftMinimap()
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->EndSwipeNow();
	}
}
void AStrategyPlayerController::MousePressedOverMinimap()
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->EndSwipeNow();
	}
}

void AStrategyPlayerController::MouseReleasedOverMinimap()
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->EndSwipeNow();
	}
}
