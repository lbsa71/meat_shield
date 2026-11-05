using UnrealBuildTool;
public class MeatShield : ModuleRules
{
    public MeatShield(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.Add(ModuleDirectory);
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AudioMixer", "PhysicsCore", "AnimGraphRuntime", "PCG", "RenderCore", "RHI", "ProceduralMeshComponent", "Json" });
    }
}
