// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SimulationSubsystem.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"
#include "Common/SimulationSettings.h"
#include "Common/Utility.h"
#include "Subsystems/SpatialGridSubsystem.h"

void USimulationSubsystem::InitializeSimulator(USimulationSettings* SimulationConfiguration)
{
	checkf(SimulationConfiguration != nullptr, TEXT("Simulation Configuration cannot be null."));
	
	Configuration = SimulationConfiguration;
	
	SpatialGrid = GetGameInstance()->GetSubsystem<USpatialGridSubsystem>();

	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}

	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}
}

UAgentData* USimulationSubsystem::AddAgent(AAgentPawn* AgentPawn)
{
	const int32 Index = AgentsData.AddUnique(NewObject<UAgentData>());
	AgentsData[Index]->SetAffectedActor(AgentPawn);
	return AgentsData[Index];
}

void USimulationSubsystem::SetChaseTarget(AActor* NewChaseTarget)
{
	ChaseTarget = NewChaseTarget;
}

void USimulationSubsystem::Simulate(float DeltaTime)
{
	for (UAgentData* Agent : AgentsData)
	{
		SenseNearbyAgents(Agent);
		ApplyBehaviourOnAgent(Agent);
		Agent->UpdateState(DeltaTime);
	}
}

void USimulationSubsystem::ApplyBehaviourOnAgent(UAgentData* TargetAgent) const
{
	FVector MovementForce = FVector::ZeroVector;

	if(CanAgentLead(TargetAgent))
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
		{
			if (const ISeekingInterface* SeekingInterface = Cast<ISeekingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += SeekingInterface->CalculateSeekForce(TargetAgent, ChaseTarget,Configuration->AgentsMaxSpeed);
			}
		}
	}
	else
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
		{
			if (const IFlockingInterface* FlockingInterface = Cast<IFlockingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += FlockingInterface->CalculateSteerForce(TargetAgent, AgentsData, Configuration->AgentsMaxSpeed);
			}
		}
	}
	
	TargetAgent->MovementForce = MovementForce;
}

void USimulationSubsystem::SenseNearbyAgents(UAgentData* TargetAgent) const
{
	TargetAgent->NearbyAgentIndices.Reset();
	if (SpatialGrid)
	{
		SpatialGrid->SearchActors(TargetAgent->Location, Configuration->AgentSenseRange,TargetAgent->NearbyAgentIndices);
	}
}

bool USimulationSubsystem::CanAgentLead(const UAgentData* TargetAgent) const
{
	if (Configuration->bForceLeadership)
	{
		return true;
	}

	const float LeadershipCheck_MaximumValue = Configuration->LeaderCheckParameters.SearchRadius.GetUpperBoundValue();
	const float LeadershipCheck_MinimumValue = Configuration->LeaderCheckParameters.SearchRadius.GetLowerBoundValue();
	const float HalfFOV = Configuration->LeaderCheckParameters.FOVHalfAngle;
	
	uint32 NumAgentsFound = 0;
	for (const uint32 Index : TargetAgent->NearbyAgentIndices)
	{
		const UAgentData* OtherAgent = AgentsData[Index];
		if (Utility::IsPointInFOV(
			TargetAgent->Location, TargetAgent->GetForwardVector(), OtherAgent->Location,
			LeadershipCheck_MinimumValue, LeadershipCheck_MaximumValue, HalfFOV))
		{
			++NumAgentsFound;
		}
	}
	return NumAgentsFound == 0;
}
