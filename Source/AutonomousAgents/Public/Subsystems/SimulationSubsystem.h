// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include "Core/Agent.h"
#include "Core/SimulationRunnable.h"
#include "SimulationSubsystem.generated.h"

// Forward declarations.
class USimulatorConfiguration;
class USpatialGridSubsystem;

/**
 * Runs core simulation logic and drives all agents.
 * Uses threads/async tasks to distribute its workload over multiple cores.
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resets the influence setting across all added behaviours.
	 */
	void ResetInfluences() const;
	/**
	 * @brief Initialize the simulator with a configuration asset.
	 * 
	 * Subsystems aren't exposed to the editors like blueprint classes,
	 * so it is important to call this function before starting the simulation
	 * to ensure that the subsystem has all the information that it needs.
	 * 
	 * @param NewConfiguration The configuration UDataAsset that the simulator must use to get all its information from. 
	 */
	UFUNCTION(BlueprintCallable)
	void Init(USimulatorConfiguration* NewConfiguration);

	/**
	 * @brief Assigns a chase target.
	 * @param NewChaseTarget Another actor that some agents may chase.
	 */
	UFUNCTION(BlueprintCallable)
	void SetChaseTarget(AActor* NewChaseTarget);
	
	/**
	 * @brief Creates a data for a new agent, and returns it.
	 * @param InitialLocation Agent's starting location.
	 * @param InitialVelocity Agent's starting velocity.
	 * @return The newly created agent.
	 */
	UAgent* CreateAgent(const FVector& InitialLocation = FVector::ZeroVector, const FVector& InitialVelocity = FVector::ZeroVector);

	// Starts Worker Threads.
	void StartSimulation();

	// TODO : Use a fixed update technique and get rid of the Tick.
	/**
	 * @brief Ticks the Simulator, useful for performing physics updates.
	 * SimulationSubsystem is not an actor and it does not tick itself. Its important to tick it manually.
	 * @param DeltaSeconds Game time elapsed during last frame modified by the time dilation
	 */
	void Tick(float DeltaSeconds) const;

	/**
	 * @brief Returns the current Transform of an agent addressed by its Index.
	 * The rotation of an Agent is calculated from its current velocity direction.
	 * @param AgentIndex Unique integer that identifies an agent.
	 * @param RotationOffset Optional Offset applied to the transform's rotation.
	 */
	FTransform GetTransform(uint32 AgentIndex, const FRotator& RotationOffset = FRotator::ZeroRotator) const;	

private:

	// Internal call to create and launch threads.
	virtual void StartSimulation_Internal();

	/**
	 * @brief Runs behaviours and sense updates on an Agent.
	 * Designed to simply the multi-threaded operations.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	void SimulationLogic(uint32 AgentIndex) const;

	/**
	 * @brief Applies behavioural logic on a certain agent identified by its index.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	virtual void ApplyBehaviourOnAgent(const uint32 AgentIndex) const;

	/**
	 * @brief Sense agents in the proximity of a certain agent identified by its index.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	virtual void SenseNearbyAgents(const uint32 AgentIndex) const;

	/**
	 * @brief Updates an Agent's physical state.
	 * 
	 * @param AgentIndex Unique integer that identifies an agent.
	 * @param DeltaSeconds Game time elapsed during last frame modified by the time dilation
	 */
	virtual void UpdateAgent(uint32 AgentIndex, float DeltaSeconds) const;

	/**
	 * @brief Perform physics update (location and velocity) on a certain agent, using a variable fixed delta-time.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	virtual void FixedUpdateAgent(const uint32 AgentIndex) const;

	/**
	 * @brief Checks all nearby agents and uses a view cone filter
	 * to determine if an agent must follow others or chase a target.
	 * 
	 * @param AgentIndex Unique integer that identifies an agent.
	 * @return 
	 */
	virtual bool ShouldAgentFlock(const uint32 AgentIndex) const;
	
private:
	
	// Reference to a UDataAsset - USimulationSettings that contains all
	// information that the SimulationSystem needs to perform its operations.
	UPROPERTY(Transient)
	USimulatorConfiguration* Configuration;

	// TODO : Replace this with an array of actors that agents can choose from.
	// Reference to a an actor that all agents that do not flock, must chase.
	UPROPERTY(Transient)
	AActor* ChaseTarget;

	// Reference to the USpatialGridSubsystem, a system that maintains an implicit grid,
	// for finding agents in a region, very quickly. 
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGrid;

	// Contains data about all agents currently being simulated.
	UPROPERTY(Transient)
	TArray<UAgent*> AgentsData;

private:

	// Thread objects that break the simulation logic into pieces and run them independently.
	TArray<TUniquePtr<FSimulationRunnable>> Runnables;
};
