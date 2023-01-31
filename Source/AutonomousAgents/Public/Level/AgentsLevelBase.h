#pragma once

#include <CoreMinimal.h>
#include <Engine/LevelScriptActor.h>

#include "AgentsLevelBase.generated.h"

// Forward declarations.
class USimulationSubsystem;
class USpatialGridSubsystem; 

/**
 * AgentsLevelBase is a Level Script Actor
 * that automates the process of spawning agents, registering them across subsystems,
 * registering chase targets and updating the visual components of the agents.
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API AAgentsLevelBase : public ALevelScriptActor
{
	
	GENERATED_BODY()
	
public:
	
	/**
	 * @brief Fetches subsystems and creates static mesh instances.
	 * Level Script Actor properties aren't exposed to the editors like blueprint classes,
	 * so it is important to call this function before starting the simulation
	 * to ensure that it has all the information that it needs.
	 * @param NewSpawnConfiguration The configuration UDataAsset that the AgentsLevelBase must use to get all its information from.
	 */
	UFUNCTION(BlueprintCallable)
	void Init(const USpawnConfiguration* NewSpawnConfiguration);

	/**
	 * @brief Spawns agents using a SpawnConfiguration, all at once.
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnAgents();

	/**
	 * @brief Calls SimulationSubsystem to start the simulation.
	 */
	UFUNCTION(BlueprintCallable)
	void StartSimulation() const;

	/**
	 * @brief Applies a multiplier to the influence setting of selected Behaviour.
	 * @param TargetBehaviour The behaviour that is to be modified.
	 * @param Scale Amount of scale to be applied.
	 */
	UFUNCTION(BlueprintCallable)
	void ScaleBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour, float Scale);

	/**
	 * @brief Resets the influence setting of selected Behaviour.
	 * @param TargetBehaviour The behaviour that is to be modified.
	 */
	UFUNCTION(BlueprintCallable)
	void ResetBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour);

public:

	// Enables tick on this actor.
	AAgentsLevelBase();

	/**
	 * @brief Updates 3 systems : Simulator, Spatial Grid, and Instanced Static Mesh Component.
	 * @param DeltaSeconds Game time elapsed during last frame modified by the time dilation
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	* @brief Loops through the static mesh instances of all agents, and updates them with their latest Transform data.
	*/
	void UpdateInstancedMeshes() const;
	
private:
	
	/**
	 * @brief Fetches references to Subsystems.
	 */
	void FetchSubsystems();

	/**
	 * @brief Calls the Simulation Subsystem to create data for a new agent,
	 * and registers this data object in the Spatial Grid Subsystem.
	 * It also creates an static mesh instance for this agent. 
	 * @param SpawnLocation The starting location of the new agent.
	 */
	void SpawnSingleAgent(FVector SpawnLocation) const;

	/**
	 * @brief Creates and registers a InstancedStaticMeshComponent.
	 */
	void CreateInstancedStaticMeshComponent();

protected:

	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGridSubsystem;

	UPROPERTY(Transient)
	USimulationSubsystem* SimulationSubsystem;
	
	UPROPERTY(Transient)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	// The configuration UDataAsset that this AgentsLevelBase Actor
	// uses to get all its configuration information from.
	UPROPERTY(Transient)
	const USpawnConfiguration* SpawnConfiguration;

private:

	// Total number of agents being simulated.
	mutable int NumAgents = 0;

	// Flag used in checks to ensure that initialization has been performed before starting the simulation.
	bool bInitialized = false;
	
};
