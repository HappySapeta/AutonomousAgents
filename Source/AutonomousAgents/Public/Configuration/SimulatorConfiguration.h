#pragma once

#include <CoreMinimal.h>
#include "Configuration/SearchParameters.h"
#include "SimulatorConfiguration.generated.h"

// Forward declarations.
class UBaseAutonomousBehaviour;

/**
 * Agent movement settings and behaviours.
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulatorConfiguration : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// Offset applied to the rotation of an Agent's static mesh.
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	FRotator RotationOffset;

	// Maximum speed of an agent.
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentsMaxSpeed = 2000.0f;

	// Agent's search radius.
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentSenseRange = 300.0f;

	// The speed at which the agent aligns itself along its velocity.
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings", meta = (UIMin = "0.001", UIMax = "1.0", ClampMin = "0.001", ClampMax = "1.0"))
	float VelocityAlignmentSpeed = 0.1f;

	// Forces all agents to be leaders and not followers.
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings")
	bool bForceLeadership = false;

	// Defines parameters that determine which agent can become a leader.
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	FSearchParameters LeaderCheckParameters;

	// Target chasing behaviours. Used by Leaders.
	UPROPERTY(EditDefaultsOnly, Category = "Behaviours", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> ChaseBehaviors;

	// Flocking behaviours. Used by followers.
	UPROPERTY(EditDefaultsOnly, Category = "Behaviours", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> FlockBehaviors;
};
