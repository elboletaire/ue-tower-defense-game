// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "SlateGameResources.h"

TSharedPtr< FSlateStyleSet > FStrategyStyle::StrategyStyleInstance = NULL;

void FStrategyStyle::Initialize()
{
	if ( !StrategyStyleInstance.IsValid() )
	{
		StrategyStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle( *StrategyStyleInstance );
	}
}

void FStrategyStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *StrategyStyleInstance );
	ensure( StrategyStyleInstance.IsUnique() );
	StrategyStyleInstance.Reset();
}

FName FStrategyStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("StrategyStyle"));
	return StyleSetName;
}


#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".otf"), __VA_ARGS__ )

TSharedRef< FSlateStyleSet > FStrategyStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FStrategyStyle::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
	FSlateStyleSet& Style = StyleRef.Get();

	const FLinearColor GoldColor(FColor(255,213,160));

	// Fonts still need to be specified in code for now
	Style.Set("StrategyGame.MenuFont", TTF_FONT("Fonts/Roboto-Black", 32));

	Style.Set("StrategyGame.ResourcesTextStyle", FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 40))
		.SetColorAndOpacity(GoldColor)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetShadowOffset(FIntPoint(-1,1))
		);

	Style.Set("StrategyGame.ButtonFont", TTF_FONT("Fonts/Roboto-Black", 18));

	return StyleRef;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FStrategyStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FStrategyStyle::Get()
{
	return *StrategyStyleInstance;
}
