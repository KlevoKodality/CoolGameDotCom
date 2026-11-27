// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CoolGameDotComEditorTarget : TargetRules
{
	public CoolGameDotComEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("CoolGameDotCom");
	}
}
