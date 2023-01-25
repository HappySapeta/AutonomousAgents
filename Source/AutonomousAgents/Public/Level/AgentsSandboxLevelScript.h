
#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "AgentsSandboxLevelScript.generated.h"

// Forward declarations
class USimulationSubsystem;
class UAgentSpawnerConfig;
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

	AAgentsSandboxLevelScript();
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnActorsImmediately(const UAgentSpawnerConfig* SpawnConfig);

	UFUNCTION(BlueprintCallable)
	void ScaleBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour, float Scale);

	UFUNCTION(BlueprintCallable)
	void ResetBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour);

	UFUNCTION(BlueprintCallable)
	void StartSimulation();

private:

	void FetchSubsystems();
	
	void SpawnActor(const UAgentSpawnerConfig* SpawnConfig, FVector SpawnLocation, FActorSpawnParameters SpawnParameters) const;
	
private:
	
	UPROPERTY(Transient)
	TObjectPtr<USpatialGridSubsystem> SpatialGridSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<USimulationSubsystem> SimulatorSubsystem;
};
