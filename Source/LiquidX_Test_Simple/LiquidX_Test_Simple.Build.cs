// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LiquidX_Test_Simple : ModuleRules
{
	public LiquidX_Test_Simple(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore" });
	}
}
