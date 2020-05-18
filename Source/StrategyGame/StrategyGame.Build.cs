// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StrategyGame : ModuleRules
{
	public StrategyGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/StrategyGame.h";

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"NavigationSystem",
				"AIModule",
				"GameplayTasks",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"StrategyGameLoadingScreen"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore",
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				"StrategyGame/Private/UI/Menu",
				"StrategyGame/Private/UI/Style",
				"StrategyGame/Private/UI/Widgets",
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			// Add UPL to add configrules.txt to our APK
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(PluginPath, "StrategyGame_UPL.xml"));
		}
	}
}
