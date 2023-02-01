

#pragma once

#include <CoreMinimal.h>
#include "Configuration/SearchParameters.h"
#include "SimulatorConfiguration.generated.h"

class UBaseAutonomousBehaviour;

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulatorConfiguration : public UDataAsset
{
	GENERATED_BODY()
	
public:
		
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	FRotator RotationOffset;
	
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentsMaxSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentSenseRange = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings")
	bool bForceLeadership = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	FSearchParameters LeaderCheckParameters;

	UPROPERTY(EditDefaultsOnly, Category = "Behaviours", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> ChaseBehaviors;
	
	UPROPERTY(EditDefaultsOnly, Category = "Behaviours", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> FlockBehaviors;
};
