// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Common/AgentData.h"
#include "Common/SimulationRunnable.h"
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

	UAgentData* CreateAgent(const FVector& InitialLocation = FVector::ZeroVector, const FVector& InitialVelocity = FVector::ZeroVector);
	
	UFUNCTION(BlueprintCallable)
	void SetChaseTarget(AActor* NewChaseTarget);
	
	void StartSimulation();
	
	int GetNumAgents() const;

	void Update(float DeltaTime);
	
	FTransform GetTransform(uint32 Index, const FRotator& RotationOffset = FRotator::ZeroRotator) const;	

private:
	
	void LaunchThreads();

	void ApplyBehaviourOnAgent(const uint32 Index) const;
	
	void SenseNearbyAgents(const uint32 Index) const;
	
	void UpdateAgent(uint32 Index, float DeltaTime);

	void FixedUpdateAgent(const uint32 Index);

	bool CanAgentLead(const UAgentData* TargetAgent) const;
	
private:

	UPROPERTY(Transient)
	USimulationSettings* SimulationSettings;
	
	UPROPERTY(Transient)
	AActor* ChaseTarget;
	
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGrid;

	UPROPERTY(Transient)
	TArray<UAgentData*> AgentsData;

private:
	
	TArray<TUniquePtr<FSimulationRunnable>> Runnables;
};
