// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/CommonTypes.h"
#include "UObject/Object.h"
#include "SimulationSubsystem.generated.h"

class USimulationSettings;
class USpatialGridSubsystem;
/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void InitializeSimulator(USimulationSettings* SimulationConfiguration);
	
	void AddAgent(const TWeakObjectPtr<AActor>& AgentActor);

	UFUNCTION(BlueprintCallable)
	void SetChaseTarget(AActor* NewChaseTarget);

	void Simulate(float DeltaTime);
	
private:

	void ApplyBehaviourOnAgent(FAgentData& TargetAgent) const;

	static void UpdateAgentState(FAgentData& TargetAgent, const float DeltaTime);

	void SenseNearbyAgents(FAgentData& TargetAgent) const;

	bool CanAgentLead(const FAgentData& TargetAgent) const;
	
private:

	UPROPERTY(Transient)
	USimulationSettings* Configuration;
	
	UPROPERTY(Transient)
	AActor* ChaseTarget;
	
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGrid;
	
	TArray<FAgentData> AgentsData;
};
