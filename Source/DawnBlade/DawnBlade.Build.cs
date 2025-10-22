// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DawnBlade : ModuleRules
{
	public DawnBlade(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HTTP",  
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"CommonUI",
			"CommonInput",
			"Json",
			"JsonUtilities",
			"GameplayAbilitiesEditor",
			"GameplayStateTreeModule",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore",
			"AdvancedWidgets"  
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// MetaHuman modules (based on your available plugins)
			"MetaHumanCore",       // Core MetaHuman systems
			"MetaHumanIdentity",   // Character identity system
			"MetaHumanPipeline",   // Asset pipeline
            
			// Animation and rigging
			"ControlRig",          // For facial animation controls
			"HairStrandsCore",     // Hair simulation
            
			// Additional useful modules
			"DeveloperSettings",   // For project settings
			"ToolMenus",          // For editor tools (if building editor tools)
			"PropertyEditor"       // For custom property editors
		});
		
		// Only include editor modules in editor builds
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"MetaHumanToolkit",       // Editor tools
				"MetaHumanSequencer",     // Sequencer integration
				"UnrealEd",
				"ToolMenus",
				"EditorStyle",
				"EditorWidgets"
			});
		}

		PublicIncludePaths.AddRange(new string[] {
			"DawnBlade",
			"DawnBlade/Variant_Platforming",
			"DawnBlade/Variant_Platforming/Animation",
			"DawnBlade/Variant_Combat",
			"DawnBlade/Variant_Combat/AI",
			"DawnBlade/Variant_Combat/Animation",
			"DawnBlade/Variant_Combat/Gameplay",
			"DawnBlade/Variant_Combat/Interfaces",
			"DawnBlade/Variant_Combat/UI",
			"DawnBlade/Variant_SideScrolling",
			"DawnBlade/Variant_SideScrolling/AI",
			"DawnBlade/Variant_SideScrolling/Gameplay",
			"DawnBlade/Variant_SideScrolling/Interfaces",
			"DawnBlade/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
