// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GGPOUE : ModuleRules
{
	public GGPOUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(new string[] { "Core", "DeveloperSettings" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "InputCore" });


		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("_WINDOWS");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
			PublicDefinitions.Add("MACOS");
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
			PublicDefinitions.Add("__GNUC__");
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
