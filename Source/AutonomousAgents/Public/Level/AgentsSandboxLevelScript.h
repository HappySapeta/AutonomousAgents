
#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "AgentsSandboxLevelScript.generated.h"

// Forward declarations
class UAgentData;
class UAgentSpawnerConfig;
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
	
	void SpawnAgent(FVector SpawnLocation) const;

protected:
	
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(Transient)
	TObjectPtr<USpatialGridSubsystem> SpatialGridSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<USimulationSubsystem> SimulatorSubsystem;

	UPROPERTY(EditDefaultsOnly)
	FRotator OffsetRotation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* AgentMesh;
	
};
