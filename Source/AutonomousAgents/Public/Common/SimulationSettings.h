// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSearchParameters.h"
#include "UObject/Object.h"
#include "SimulationSettings.generated.h"

class UBaseAutonomousBehaviour;

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulationSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Simulation Settings", meta = (UIMin = "1", UIMax = "32", ClampMin = "0", ClampMax = "32", Delta = "1"))
	uint32 ThreadCount = 6;

	UPROPERTY(EditDefaultsOnly, Category = "Simulation Settings")
	float FixedDeltaTime = 0.016f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentsMaxSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Agent Settings")
	float AgentSenseRange = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings")
	bool bForceLeadership = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Leader Check Settings", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	FSearchParameters LeaderCheckParameters;

	UPROPERTY(EditDefaultsOnly, Category = "Behaviours")
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> ChaseBehaviors;
	
	UPROPERTY(EditDefaultsOnly, Category = "Behaviours")
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> FlockBehaviors;
};
