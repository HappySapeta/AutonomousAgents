// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Common/AgentData.h"
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

	TWeakPtr<FAgentData> AddAgent(const TWeakObjectPtr<AActor>& AgentActor);

	UFUNCTION(BlueprintCallable)
	void SetChaseTarget(AActor* NewChaseTarget);

	void Simulate(float DeltaTime);
	
private:

	void ApplyBehaviourOnAgent(const TSharedPtr<FAgentData>& TargetAgent) const;

	static void UpdateAgentState(const TSharedPtr<FAgentData>& TargetAgent, const float DeltaTime);

	void SenseNearbyAgents(const TSharedPtr<FAgentData>& TargetAgent) const;

	bool CanAgentLead(const TSharedPtr<FAgentData>& TargetAgent) const;
	
private:

	UPROPERTY(Transient)
	USimulationSettings* Configuration;
	
	UPROPERTY(Transient)
	AActor* ChaseTarget;
	
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGrid;
	
	TArray<TSharedPtr<FAgentData>> AgentsData;
	TArray<const FAgentData*> AgentData_Ptrs;
};
