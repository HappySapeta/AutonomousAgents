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

TWeakPtr<FAgentData> USimulationSubsystem::AddAgent(const TWeakObjectPtr<AActor>& AgentActor)
{
	const int32 Index = AgentsData.AddUnique(MakeShared<FAgentData>(AgentActor));
	if(Index >= 0)
	{
		AgentData_Ptrs.Add(AgentsData.Last().Get());
		return AgentsData[Index];
	}

	return nullptr;
}

void USimulationSubsystem::SetChaseTarget(AActor* NewChaseTarget)
{
	ChaseTarget = NewChaseTarget;
}

void USimulationSubsystem::Simulate(float DeltaTime)
{
	for (TSharedPtr<FAgentData>& Agent : AgentsData)
	{
		SenseNearbyAgents(Agent);
		ApplyBehaviourOnAgent(Agent);
		UpdateAgentState(Agent, DeltaTime);
	}
}

void USimulationSubsystem::ApplyBehaviourOnAgent(const TSharedPtr<FAgentData>& TargetAgent) const
{
	FVector MovementForce = FVector::ZeroVector;

	if(CanAgentLead(TargetAgent))
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
		{
			if (const ISeekingInterface* SeekingInterface = Cast<ISeekingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += SeekingInterface->CalculateSeekForce(TargetAgent.Get(), ChaseTarget,Configuration->AgentsMaxSpeed);
			}
		}
	}
	else
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
		{
			if (const IFlockingInterface* FlockingInterface = Cast<IFlockingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += FlockingInterface->CalculateSteerForce(TargetAgent.Get(), AgentData_Ptrs, Configuration->AgentsMaxSpeed);
			}
		}
	}
	
	TargetAgent->MovementForce = MovementForce;
}

void USimulationSubsystem::UpdateAgentState(const TSharedPtr<FAgentData>& TargetAgent, const float DeltaTime)
{
	const FVector& NewVelocity = TargetAgent->Velocity + TargetAgent->MovementForce * DeltaTime;
	const FVector& NewLocation = TargetAgent->Location + NewVelocity * DeltaTime;

	TargetAgent->Velocity = NewVelocity;
	TargetAgent->Location = NewLocation;
	TargetAgent->MovementForce = FVector::ZeroVector;
}

void USimulationSubsystem::SenseNearbyAgents(const TSharedPtr<FAgentData>& TargetAgent) const
{
	TargetAgent->NearbyAgentIndices.Reset();
	if (SpatialGrid)
	{
		SpatialGrid->SearchActors(TargetAgent->Location, Configuration->AgentSenseRange,TargetAgent->NearbyAgentIndices);
	}
}

bool USimulationSubsystem::CanAgentLead(const TSharedPtr<FAgentData>& TargetAgent) const
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
		const TSharedPtr<FAgentData>& OtherAgent = AgentsData[Index];
		if (Utility::IsPointInFOV(
			TargetAgent->Location, TargetAgent->GetForwardVector(), OtherAgent->Location,
			LeadershipCheck_MinimumValue, LeadershipCheck_MaximumValue, HalfFOV))
		{
			++NumAgentsFound;
		}
	}
	return NumAgentsFound == 0;
}
