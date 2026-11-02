using UnrealBuildTool;
public class MeatShieldEditorTarget : TargetRules
{
    public MeatShieldEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("MeatShield");
    }
}
