

using UnrealBuildTool;
using System.Collections.Generic;

public class AutonomousAgentsTarget : TargetRules
{
	public AutonomousAgentsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "AutonomousAgents" } );
	}
}
