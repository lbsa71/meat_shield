using UnrealBuildTool;
public class MeatShieldTarget : TargetRules
{
    public MeatShieldTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("MeatShield");
    }
}
