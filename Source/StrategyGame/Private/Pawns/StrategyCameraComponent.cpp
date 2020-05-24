// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyCameraComponent.h"
#include "StrategyHelpers.h"
#include "StrategySpectatorPawnMovement.h"
#include "StrategyInput.h"

UStrategyCameraComponent::UStrategyCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ZoomAlpha = 1.0f;
	CameraScrollSpeed = 4000.0f;
	MinZoomLevel = 0.4f;
	MaxZoomLevel = 1.0f;
	MiniMapBoundsLimit = 0.8f;
	StartSwipeCoords.Set(0.0f, 0.0f, 0.0f);
}

void UStrategyCameraComponent::OnZoomIn()
{
	SetZoomLevel(ZoomAlpha - 0.1f);
}

void UStrategyCameraComponent::OnZoomOut()
{
	SetZoomLevel(ZoomAlpha + 0.1f);
}

void UStrategyCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& OutResult)
{
	APlayerController* Controller = GetPlayerController();
	if (Controller)
	{
		OutResult.FOV = 30.f;
		const float CurrentOffset = MinCameraOffset + ZoomAlpha * (MaxCameraOffset - MinCameraOffset);
		FVector Pos2 = Controller->GetFocalLocation();
		OutResult.Location = Controller->GetFocalLocation() - FixedCameraAngle.Vector() * CurrentOffset;
		OutResult.Rotation = FixedCameraAngle;
	}
}

void UStrategyCameraComponent::UpdateCameraMovement( const APlayerController* InPlayerController )
{
	// No mouse support on mobile
#if PLATFORM_DESKTOP
	ULocalPlayer* const LocalPlayer =  Cast<ULocalPlayer>(InPlayerController->Player);
	if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport )
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition) == false)
		{
			return;
		}

		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		const float ScrollSpeed = 60.0f;
		const FIntPoint ViewportSize = Viewport->GetSizeXY();
		const uint32 ViewLeft = FMath::TruncToInt(LocalPlayer->Origin.X * ViewportSize.X);
		const uint32 ViewRight = ViewLeft + FMath::TruncToInt(LocalPlayer->Size.X * ViewportSize.X);
		const uint32 ViewTop = FMath::TruncToInt(LocalPlayer->Origin.Y * ViewportSize.Y);
		const uint32 ViewBottom = ViewTop + FMath::TruncToInt(LocalPlayer->Size.Y * ViewportSize.Y);

		const float MaxSpeed = CameraScrollSpeed * FMath::Clamp(ZoomAlpha, 0.3f, 1.0f);

		AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
		bool bNoScrollZone = false;
		FVector MouseCoords( MousePosition, 0.0f );
		for (int iZone = 0; iZone < NoScrollZones.Num() ; iZone++)
		{
			FBox EachZone = NoScrollZones[ iZone ];
			if( EachZone.IsInsideXY( MouseCoords ) == true )
			{
				bNoScrollZone = true;
			}
		}

		const uint32 MouseX = MousePosition.X;
		const uint32 MouseY = MousePosition.Y;
		float SpectatorCameraSpeed = MaxSpeed;
		ASpectatorPawn* SpectatorPawn = NULL;
		if( GetPlayerController() != NULL )
		{
			SpectatorPawn = GetPlayerController()->GetSpectatorPawn();
			if( SpectatorPawn->GetMovementComponent() != NULL )
			{
				SpectatorCameraSpeed = GetDefault<UStrategySpectatorPawnMovement>(SpectatorPawn->GetMovementComponent()->GetClass())->MaxSpeed;
			}
		}
		if (!bNoScrollZone)
		{
			if (MouseX >= ViewLeft && MouseX <= (ViewLeft + CameraActiveBorder))
			{
				const float delta = 1.0f - float(MouseX - ViewLeft) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveRight(-ScrollSpeed * delta);
			}
			else if (MouseX >= (ViewRight - CameraActiveBorder) && MouseX <= ViewRight)
			{
				const float delta = float(MouseX - ViewRight + CameraActiveBorder) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveRight(ScrollSpeed * delta);
			}

			if (MouseY >= ViewTop && MouseY <= (ViewTop + CameraActiveBorder))
			{
				const float delta = 1.0f - float(MouseY - ViewTop) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveForward(ScrollSpeed * delta);
			}
			else if (MouseY >= (ViewBottom - CameraActiveBorder) && MouseY <= ViewBottom)
			{
				const float delta = float(MouseY - (ViewBottom - CameraActiveBorder)) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveForward(-ScrollSpeed * delta);
			}

			if( SpectatorPawn != NULL )
			{
				UFloatingPawnMovement* PawnMovementComponent = Cast<UFloatingPawnMovement>(SpectatorPawn->GetMovementComponent());
				if (PawnMovementComponent)
				{
					PawnMovementComponent->MaxSpeed = SpectatorCameraSpeed;
				}
			}
		}
	}
#endif
	NoScrollZones.Empty();
}

void UStrategyCameraComponent::MoveForward(float Val)
{
	APawn* OwnerPawn = GetOwnerPawn();
	if( OwnerPawn != NULL )
	{
		APlayerController* Controller = GetPlayerController();
		if( (Val != 0.f) && ( Controller != NULL ))
		{
			const FRotationMatrix R(Controller->PlayerCameraManager->GetCameraRotation());
			const FVector WorldSpaceAccel = R.GetScaledAxis( EAxis::X ) * 100.0f;

			// transform to world space and add it
			OwnerPawn->AddMovementInput(WorldSpaceAccel, Val);
		}
	}
}

void UStrategyCameraComponent::MoveRight(float Val)
{
	APawn* OwnerPawn = GetOwnerPawn();
	if( OwnerPawn != NULL )
	{
		APlayerController* Controller = GetPlayerController();
		if( (Val != 0.f) && ( Controller != NULL ))
		{
			const FRotationMatrix R(Controller->PlayerCameraManager->GetCameraRotation());
			const FVector WorldSpaceAccel = R.GetScaledAxis( EAxis::Y ) * 100.0f;

			// transform to world space and add it
			OwnerPawn->AddMovementInput(WorldSpaceAccel, Val);
		}
	}
}

void UStrategyCameraComponent::AddNoScrollZone( FBox InCoords )
{
	NoScrollZones.AddUnique( InCoords );
}

void UStrategyCameraComponent::ClampCameraLocation( const APlayerController* InPlayerController, FVector& OutCameraLocation )
{
	if (bShouldClampCamera)
	{
		UpdateCameraBounds(InPlayerController);
		if (CameraMovementBounds.GetSize() != FVector::ZeroVector)
		{
			OutCameraLocation = CameraMovementBounds.GetClosestPointTo(OutCameraLocation);
		}
	}
}

void UStrategyCameraComponent::UpdateCameraBounds( const APlayerController* InPlayerController )
{
	ULocalPlayer* const LocalPlayer =  Cast<ULocalPlayer>(InPlayerController->Player);
	if (LocalPlayer == NULL || LocalPlayer->ViewportClient == NULL)
	{
		return;
	}

	FVector2D CurrentViewportSize;
	LocalPlayer->ViewportClient->GetViewportSize(CurrentViewportSize);

	// calc frustum edge direction, from bottom left corner
	if (CameraMovementBounds.GetSize() == FVector::ZeroVector || CurrentViewportSize != CameraMovementViewportSize)
	{
		// calc frustum edge direction, from bottom left corner
		const FVector FrustumRay2DDir = FVector(1,1,0).GetSafeNormal();
		const FVector FrustumRay2DRight = FVector::CrossProduct(FrustumRay2DDir, FVector::UpVector);
		const FQuat RotQuat(FrustumRay2DRight, FMath::DegreesToRadians(90.0f - InPlayerController->PlayerCameraManager->GetFOVAngle() * 0.5f));
		const FVector FrustumRayDir = RotQuat.RotateVector(FrustumRay2DDir);

		// collect 3 world bounds' points and matching frustum rays (bottom left, top left, bottom right)
		AStrategyGameState const* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (MyGameState)
		{
			FBox const& WorldBounds = MyGameState->WorldBounds;

			if (WorldBounds.GetSize() != FVector::ZeroVector)
			{
				const FVector WorldBoundPoints[] = {
					FVector(WorldBounds.Min.X, WorldBounds.Min.Y, WorldBounds.Max.Z),
					FVector(WorldBounds.Min.X, WorldBounds.Max.Y, WorldBounds.Max.Z),
					FVector(WorldBounds.Max.X, WorldBounds.Min.Y, WorldBounds.Max.Z)
				};
				const FVector FrustumRays[] = {
					FVector( FrustumRayDir.X,  FrustumRayDir.Y, FrustumRayDir.Z),
					FVector( FrustumRayDir.X, -FrustumRayDir.Y, FrustumRayDir.Z),
					FVector(-FrustumRayDir.X,  FrustumRayDir.Y, FrustumRayDir.Z)
				};

				// get camera plane for intersections
				const FPlane CameraPlane = FPlane(InPlayerController->GetFocalLocation(), FVector::UpVector);

				// get matching points on camera plane
				const FVector CameraPlanePoints[3] = {
					FStrategyHelpers::IntersectRayWithPlane(WorldBoundPoints[0], FrustumRays[0], CameraPlane)*MiniMapBoundsLimit,
					FStrategyHelpers::IntersectRayWithPlane(WorldBoundPoints[1], FrustumRays[1], CameraPlane)*MiniMapBoundsLimit,
					FStrategyHelpers::IntersectRayWithPlane(WorldBoundPoints[2], FrustumRays[2], CameraPlane)*MiniMapBoundsLimit
				};

				// create new bounds
				CameraMovementBounds = FBox(CameraPlanePoints, 3);
				CameraMovementViewportSize = CurrentViewportSize;
			}
		}
	}
}

APlayerController* UStrategyCameraComponent::GetPlayerController()
{
	APlayerController* Controller = NULL;
	APawn* Owner = GetOwnerPawn();
	if( Owner != NULL )
	{
		Controller = Cast<APlayerController>(Owner->GetController());
	}
	return Controller;
}

APawn* UStrategyCameraComponent::GetOwnerPawn()
{
	return Cast<APawn>(GetOwner());
}

void UStrategyCameraComponent::OnPinchStarted(const FVector2D& AnchorPosition1, const FVector2D& AnchorPosition2, float DownTime)
{
	InitialPinchAlpha = ZoomAlpha;
}

static TAutoConsoleVariable<float> CVarPinchScale(TEXT("PinchScale"), 0.002f, TEXT("How fast strategy pinch is."));

void UStrategyCameraComponent::OnPinchUpdate(UStrategyInput* InputHandler, const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	const float AnchorDistance = (InputHandler->GetTouchAnchor(0) - InputHandler->GetTouchAnchor(1)).Size();
	const float CurrentDistance = (ScreenPosition1 - ScreenPosition2).Size();
	const float PinchDelta = AnchorDistance - CurrentDistance;
	const float PinchScale = CVarPinchScale.GetValueOnGameThread();

	SetZoomLevel(InitialPinchAlpha + PinchDelta * PinchScale);
}

void UStrategyCameraComponent::SetCameraTarget(const FVector& CameraTarget)
{
	ASpectatorPawn* SpectatorPawn = GetPlayerController()->GetSpectatorPawn();
	if( SpectatorPawn != NULL )
	{
		SpectatorPawn->SetActorLocation(CameraTarget, false);
	}
}

void UStrategyCameraComponent::SetZoomLevel(float NewLevel)
{
	ZoomAlpha = FMath::Clamp(NewLevel, MinZoomLevel, MaxZoomLevel);
}

bool UStrategyCameraComponent::OnSwipeStarted(const FVector2D& SwipePosition)
{
	bool bResult = false;
	// Ensure we are NOT trying to start a drag/scroll over a no scroll zone (EG mini map)
	if (AreCoordsInNoScrollZone(SwipePosition) == false)
	{
		APlayerController* Controller = GetPlayerController();
		if (Controller)
		{
			// Get intersection point with the plan used to move around
			FHitResult Hit;
			if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
			{
				StartSwipeCoords = Hit.ImpactPoint;
				bResult = true;
			}
		}
	}
	else
	{
		EndSwipeNow();
	}
	return bResult;
}

bool UStrategyCameraComponent::OnSwipeUpdate(const FVector2D& SwipePosition)
{
	bool bResult = false;
	APlayerController* Controller = GetPlayerController();
	if ((Controller != NULL) && (StartSwipeCoords.IsNearlyZero() == false ) )
	{
		FHitResult Hit;
		if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
		{
			FVector NewSwipeCoords = Hit.ImpactPoint;
			FVector Delta = StartSwipeCoords - NewSwipeCoords;
			// Flatten Z axis - we are not interested in that.
			Delta.Z = 0.0f;
			if (Delta.IsNearlyZero() == false)
			{
				ASpectatorPawn* SpectatorPawn = GetPlayerController()->GetSpectatorPawn();
				if (SpectatorPawn != NULL)
				{
					FVector CurrentCamera = SpectatorPawn->GetActorLocation();
					CurrentCamera += Delta;
					SetCameraTarget(CurrentCamera);
					bResult = true;
				}
			}
		}
	}
	return bResult;
}

bool UStrategyCameraComponent::OnSwipeReleased(const FVector2D& SwipePosition)
{
	bool bResult = false;
	if (StartSwipeCoords.IsNearlyZero() == false)
	{
		APlayerController* Controller = GetPlayerController();
		if (Controller)
		{
			FHitResult Hit;
			if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
			{
				FVector EndSwipeCoords = Hit.ImpactPoint;
				bResult = true;
			}
		}
		EndSwipeNow();
	}

	return false;
}

void UStrategyCameraComponent::EndSwipeNow()
{
	StartSwipeCoords.Set(0.0f, 0.0f, 0.0f);
}

bool UStrategyCameraComponent::AreCoordsInNoScrollZone(const FVector2D& SwipePosition)
{
	bool bResult = false;
	FVector MouseCoords(SwipePosition, 0.0f);
	for (int iZone = 0; iZone < NoScrollZones.Num(); iZone++)
	{
		FBox EachZone = NoScrollZones[iZone];
		if (EachZone.IsInsideXY(MouseCoords) == true)
		{
			bResult = true;
		}
	}
	return bResult;
}
