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

	UAgentData* AddAgent(AAgentPawn* AgentPawn);

	UFUNCTION(BlueprintCallable)
	void SetChaseTarget(AActor* NewChaseTarget);

	void Simulate(float DeltaTime);
	
private:

	void ApplyBehaviourOnAgent(UAgentData* TargetAgent) const;
	
	void SenseNearbyAgents(UAgentData* TargetAgent) const;

	bool CanAgentLead(const UAgentData* TargetAgent) const;
	
private:

	UPROPERTY(Transient)
	USimulationSettings* Configuration;
	
	UPROPERTY(Transient)
	AActor* ChaseTarget;
	
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGrid;

	UPROPERTY(Transient)
	TArray<UAgentData*> AgentsData;
};
