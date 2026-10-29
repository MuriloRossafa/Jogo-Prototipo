using UnrealBuildTool;
public class OtherSideEditorTarget : TargetRules
{
    public OtherSideEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("OtherSide");
    }
}
