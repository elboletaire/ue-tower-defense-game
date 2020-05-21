// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StrategyGameEditorTarget : TargetRules
{
	public StrategyGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        //DefaultBuildSettings = BuildSettingsVersion.V2;
        bLegacyPublicIncludePaths = true;
        //PCHUsage = PCHUsageMode.UseSharedPCHs;
        ExtraModuleNames.Add("StrategyGame");
	}
}
