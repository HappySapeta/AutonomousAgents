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
	
	SimulationSettings = SimulationConfiguration;
	
	SpatialGrid = GetGameInstance()->GetSubsystem<USpatialGridSubsystem>();

	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : SimulationSettings->ChaseBehaviors)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}

	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : SimulationSettings->FlockBehaviors)
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

void USimulationSubsystem::LaunchThreads()
{
	int LowerLimit = 0;
	const uint32 BucketSize = FMath::CeilToInt32(AgentsData.Num() / static_cast<float>(SimulationSettings->ThreadCount));
	
	for(uint32 ThreadIndex = 0; ThreadIndex < SimulationSettings->ThreadCount; ++ThreadIndex)
	{
		int UpperLimit = FMath::Min(AgentsData.Num() - 1, static_cast<int>(LowerLimit + BucketSize - 1));
		const FString& ThreadName = "SimulationThread" + ThreadIndex;
		FRunData Data(LowerLimit, LowerLimit + BucketSize - 1,
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::SenseNearbyAgents),
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::ApplyBehaviourOnAgent),
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::UpdateState),
		              ThreadName);
		
		Runnables.Add(MakeUnique<FSimulationRunnable>(Data));

		LowerLimit += BucketSize;
	}
}

void USimulationSubsystem::StartSimulation()
{
	LaunchThreads();
}

void USimulationSubsystem::Tick(float DeltaTime)
{
	for(UAgentData* Agent : AgentsData)
	{
		Agent->UpdateState(DeltaTime);
	}
}

void USimulationSubsystem::ApplyBehaviourOnAgent(const uint32 Index) const
{
	if(!AgentsData.IsValidIndex(Index))
	{
		return;
	}

	UAgentData* TargetAgent = AgentsData[Index];
	
	FVector MovementForce = FVector::ZeroVector;
	if(CanAgentLead(TargetAgent))
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : SimulationSettings->ChaseBehaviors)
		{
			if (const ISeekingInterface* SeekingInterface = Cast<ISeekingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += SeekingInterface->CalculateSeekForce(TargetAgent, ChaseTarget,SimulationSettings->AgentsMaxSpeed);
			}
		}
	}
	else
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : SimulationSettings->FlockBehaviors)
		{
			if (const IFlockingInterface* FlockingInterface = Cast<IFlockingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += FlockingInterface->CalculateSteerForce(TargetAgent, AgentsData, SimulationSettings->AgentsMaxSpeed);
			}
		}
	}
	
	TargetAgent->MovementForce = MovementForce;
}

void USimulationSubsystem::SenseNearbyAgents(const uint32 Index) const
{
	if(!AgentsData.IsValidIndex(Index))
	{
		return;
	}

	UAgentData* TargetAgent = AgentsData[Index];
	TargetAgent->NearbyAgentIndices.Reset();
	if (SpatialGrid)
	{
		SpatialGrid->SearchActors(TargetAgent->Location, SimulationSettings->AgentSenseRange,TargetAgent->NearbyAgentIndices);
	}
}

void USimulationSubsystem::UpdateState(const uint32 Index)
{
	if(!AgentsData.IsValidIndex(Index))
	{
		return;
	}

	UAgentData* TargetAgent = AgentsData[Index];
	TargetAgent->UpdateState(SimulationSettings->FixedDeltaTime);
}

bool USimulationSubsystem::CanAgentLead(const UAgentData* TargetAgent) const
{
	if (SimulationSettings->bForceLeadership)
	{
		return true;
	}

	const float LeadershipCheck_MaximumValue = SimulationSettings->LeaderCheckParameters.SearchRadius.GetUpperBoundValue();
	const float LeadershipCheck_MinimumValue = SimulationSettings->LeaderCheckParameters.SearchRadius.GetLowerBoundValue();
	const float HalfFOV = SimulationSettings->LeaderCheckParameters.FOVHalfAngle;
	
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
