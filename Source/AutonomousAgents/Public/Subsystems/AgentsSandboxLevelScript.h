// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "AgentsSandboxLevelScript.generated.h"

class UBaseAutonomousBehaviour;
// Forward declarations
class UAgentSpawnerConfig;
class AAgentPawn;

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API AAgentsSandboxLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SpawnActorsImmediately(const UAgentSpawnerConfig* SpawnConfig);

	UFUNCTION(BlueprintCallable)
	void DestroyAllSpawnedActors();

	UFUNCTION(BlueprintCallable)
	void ScaleBehaviourInfluence(const TSubclassOf<UBaseAutonomousBehaviour>& BehaviourType, float Scale);
	
private:
	
	TArray<TWeakObjectPtr<AAgentPawn>> SpawnedAgents;
	
};
