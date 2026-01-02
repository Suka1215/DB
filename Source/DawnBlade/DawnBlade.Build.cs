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
          "GameplayStateTreeModule",
          "NavigationSystem",
          "GameplayAbilities",
          "GameplayTags",
          "GameplayTasks",
          "UMG",
          "Slate",
          "SlateCore",
          "AudioMixer",
          "AdvancedWidgets",
          "Niagara"
       });

       PrivateDependencyModuleNames.AddRange(new string[]
       {
          // Animation and rigging (non-MetaHuman)
          "ControlRig",          // For facial animation controls
          "HairStrandsCore",     // Hair simulation
            
          // Additional useful modules
          "DeveloperSettings"    // For project settings
       });
       
       // Only include editor/MetaHuman modules in editor builds
       // Server doesn't need MetaHuman rendering
       if (Target.bBuildEditor)
       {
          PrivateDependencyModuleNames.AddRange(new string[]
          {
             // MetaHuman modules - Editor only
             "MetaHumanCore",
             "MetaHumanIdentity",
             "MetaHumanPipeline",
             "MetaHumanToolkit",
             "MetaHumanSequencer",
             
             // Editor modules
             "GameplayAbilitiesEditor",
             "UnrealEd",
             "ToolMenus",
             "PropertyEditor",
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
    }
}