// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/AgentsSandboxLevelScript.h"
#include "Common/AgentSpawnerConfig.h"
#include "Core/AgentPawn.h"

void AAgentsSandboxLevelScript::SpawnActorsImmediately(const UAgentSpawnerConfig* SpawnConfig)
{
	if(SpawnConfig == nullptr) return;
	if(SpawnConfig->AgentClass == nullptr) return;
	
	UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	FVector SpawnLocation = SpawnConfig->Origin - FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);

	const FVector& LowerBound = SpawnConfig->Origin - FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);
	const FVector& UpperBound = SpawnConfig->Origin + FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);

	FActorSpawnParameters SpawnParameters;
#if UE_EDITOR
	SpawnParameters.bHideFromSceneOutliner = SpawnConfig->bHideFromSceneOutliner;
#endif
	SpawnParameters.ObjectFlags |= RF_Transient;
	
	while(SpawnLocation.X >= LowerBound.X && SpawnLocation.X <= UpperBound.X)
	{
		while(SpawnLocation.Y >= LowerBound.Y && SpawnLocation.Y <= UpperBound.Y)
		{
			const TWeakObjectPtr<AAgentPawn>& NewAgent = Cast<AAgentPawn>(World->SpawnActor(SpawnConfig->AgentClass, &SpawnLocation, &FRotator::ZeroRotator, SpawnParameters));
			SpawnedAgents.Add(NewAgent);
			SpawnLocation.Y += SpawnConfig->Separation;
		}
		SpawnLocation.X += SpawnConfig->Separation;
		SpawnLocation.Y = SpawnConfig->Origin.Y - SpawnConfig->Span;
	}
}

void AAgentsSandboxLevelScript::DestroyAllSpawnedActors()
{
	for(TWeakObjectPtr<AAgentPawn>& Agent : SpawnedAgents)
	{
		if(!Agent.IsValid())
		{
			continue;
		}
		Agent->Destroy();
	}
	SpawnedAgents.Reset();
}

void AAgentsSandboxLevelScript::ScaleBehaviourInfluence(const TSubclassOf<UBaseAutonomousBehaviour>& BehaviourType, float Scale)
{
	
}
