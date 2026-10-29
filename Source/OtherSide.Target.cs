using UnrealBuildTool;
public class OtherSideTarget : TargetRules
{
    public OtherSideTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("OtherSide");
    }
}
