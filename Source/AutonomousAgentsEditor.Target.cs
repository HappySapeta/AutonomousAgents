

using UnrealBuildTool;
using System.Collections.Generic;

public class AutonomousAgentsEditorTarget : TargetRules
{
	public AutonomousAgentsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "AutonomousAgents" } );
	}
}
