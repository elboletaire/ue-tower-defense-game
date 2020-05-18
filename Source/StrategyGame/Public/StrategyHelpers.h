// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyMenuItem.h"
#include "SStrategyMenuItem.h"


#pragma once


class FStrategyHelpers
{
public:
	/** convert point in screen space to ray in world space */
	static bool DeprojectScreenToWorld(const FVector2D& ScreenPosition, class ULocalPlayer* Player, FVector& RayOrigin, FVector& RayDirection);

	/** find intersection of ray in world space with ground plane */
	static FVector IntersectRayWithPlane(const FVector& RayOrigin, const FVector& RayDirection, const FPlane& Plane);

	/** create alpha map from UTexture2D for hit-tests in Slate */
	static TSharedPtr<TArray<uint8>> CreateAlphaMapFromTexture(UTexture2D* Texture);

	/** creates FCanvasUVTri without UV from 3x FVector2D */
	static FCanvasUVTri CreateCanvasTri(FVector2D V0, FVector2D V1,FVector2D V2);
};

namespace MenuHelper
{

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FStrategyMenuItem::FOnConfirmMenuItem::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateUObject(inObj, inMethod),SoundDuration, false);

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FStrategyMenuItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod),SoundDuration, false);

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FStrategyMenuItem::FOnConfirmMenuItem::TSPMethodDelegate_Const< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod),SoundDuration, false);

		return Result;
	}

	FORCEINLINE float GetSoundPlaybackPosition(UWorld* World, const FSlateSound& Sound, FTimerHandle inHandle)
	{
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		return World->GetTimerManager().GetTimerElapsed(inHandle) / SoundDuration;
	}
}
