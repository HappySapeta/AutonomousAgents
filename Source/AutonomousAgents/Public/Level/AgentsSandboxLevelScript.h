
#pragma once

#include <CoreMinimal.h>
#include <Engine/LevelScriptActor.h>

#include "AgentsSandboxLevelScript.generated.h"

// Forward declarations.
class UAgent;
class USpawnConfiguration;
class USimulationSubsystem;
class USpatialGridSubsystem;
class UBaseAutonomousBehaviour;

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API AAgentsSandboxLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void Init(const USpawnConfiguration* Configuration);

	UFUNCTION(BlueprintCallable)
	void SpawnAgents();
	
	UFUNCTION(BlueprintCallable)
	void StartSimulation();

	UFUNCTION(BlueprintCallable)
	void ScaleBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour, float Scale);

	UFUNCTION(BlueprintCallable)
	void ResetBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour);

public:

	AAgentsSandboxLevelScript();

	virtual void Tick(float DeltaSeconds) override;
	
private:

	void FetchSubsystems();
	
	void SpawnSingleAgent(FVector SpawnLocation) const;

	void UpdateInstancedMeshes() const;
	
	void CreateInstancedStaticMeshComponent();

protected:
	
	UPROPERTY(Transient)
	USpatialGridSubsystem* SpatialGridSubsystem;

	UPROPERTY(Transient)
	USimulationSubsystem* SimulatorSubsystem;
	
	UPROPERTY(Transient)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	UPROPERTY(Transient)
	const USpawnConfiguration* SpawnConfiguration;

private:

	mutable int NumAgents = 0;

	bool bInitialized = false;
};
