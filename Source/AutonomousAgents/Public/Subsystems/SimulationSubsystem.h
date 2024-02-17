#pragma once

#include "CoreMinimal.h"
#include "Core/Agent.h"
#include "SpatialAcceleration/RpImplicitGrid.h"
#include "SimulationSubsystem.generated.h"

// Forward declarations.
class USimulatorConfiguration;

/**
 * Runs core simulation logic and drives all agents.
 * Uses threads/async tasks to distribute its workload over multiple cores.
 */
UCLASS()
class AUTONOMOUSAGENTS_API USimulationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DELEGATE_TwoParams(FOnAgentUpdatedEvent, uint32, const FTransform&);
	
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
	
	void Tick(float DeltaTime);

	/**
	 * @brief Returns the current Transform of an agent addressed by its Index.
	 * The rotation of an Agent is calculated from its current velocity direction.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	FTransform GetTransform(const uint32 AgentIndex) const;

	/**
	 * @brief Returns an array that contains updated transform objects of all Agents.
	 * @return Array of Transforms.
	 */
	const TArray<FTransform>& USimulationSubsystem::GetTransforms() const;

private:

	void UpdateTransform(uint32 AgentIndex);
	
	/**
	 * @brief Runs behaviours and sense updates on an Agent.
	 * Designed to simply the multi-threaded operations.
	 * @param AgentIndex Unique integer that identifies an agent.
	 */
	void RunSimulationLogicOnSingleAgent(const uint32 AgentIndex) const;

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
	virtual void UpdateAgent(uint32 AgentIndex, float DeltaSeconds);
	
	/**
	 * @brief Checks all nearby agents and uses a view cone filter
	 * to determine if an agent must follow others or chase a target.
	 * 
	 * @param AgentIndex Unique integer that identifies an agent.
	 * @return 
	 */
	virtual bool ShouldAgentFlock(const uint32 AgentIndex) const;
	
private:
	
	/**
	* Reference to a UDataAsset - USimulationSettings that contains all
	* information that the SimulationSystem needs to perform its operations.
	*/
	UPROPERTY(Transient)
	USimulatorConfiguration* Configuration;

	/**
	* @brief TODO : Replace this with an array of actors that agents can choose from.
	* Reference to a an actor that all agents that do not flock, must chase.
	*/
	UPROPERTY(Transient)
	AActor* ChaseTarget;

	/**
	* @brief Reference to the USpatialGridSubsystem, a system that maintains an implicit grid,
	* for finding agents in a region, very quickly. 
	*/
	FRpImplicitGrid ImplicitGrid;

	// Contains data about all agents currently being simulated.
	UPROPERTY(Transient)
	TArray<UAgent*> AgentsData;
		
	// Array that contains Transform information of all agents.
	TArray<FTransform> AgentTransforms;
};
