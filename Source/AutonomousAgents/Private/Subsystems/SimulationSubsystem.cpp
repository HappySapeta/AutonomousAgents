﻿#include "Subsystems/SimulationSubsystem.h"

#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"

#include "Kismet/KismetMathLibrary.h"

#include "Common/Utility.h"
#include "Configuration/SimulatorConfiguration.h"

// TODO: This is a temporary patch to a problem where we can't prevent CDOs from serializing certain variables. This function shouldn't exist.
void USimulationSubsystem::ResetInfluences() const
{
	for (TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
	{
		// CDO always serialize influence parameters, it is important to reset them before starting the simulation.
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}

	for (TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
	{
		// CDO always serialize influence parameters, it is important to reset them before starting the simulation.
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}
}

void USimulationSubsystem::Init(USimulatorConfiguration* NewConfiguration)
{
	checkf(NewConfiguration != nullptr, TEXT("Simulation Configuration cannot be null."));
	Configuration = NewConfiguration;
	ResetInfluences();
}

// TODO : I need to find a way to run the loop without relying on Tick.
void USimulationSubsystem::Tick(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USimulationSubsystem::Tick)
	ParallelFor
	(
		AgentsData.Num(),
		[this, DeltaTime](const int32 Index)
		{
			RunSimulationLogicOnSingleAgent(Index);
			UpdateAgent(Index, DeltaTime);
			UpdateTransform(Index);
		}
	);
}

UAgent* USimulationSubsystem::CreateAgent(const FVector& InitialLocation, const FVector& InitialVelocity)
{
	AgentTransforms.Add(FTransform::Identity);

	UAgent* NewAgent = NewObject<UAgent>();
	AgentsData.Add(NewAgent);
	
	NewAgent->Location = InitialLocation;
	NewAgent->Velocity = InitialVelocity;
	NewAgent->SetVelocityAlignmentSpeed(Configuration->VelocityAlignmentSpeed);

	return NewAgent;
}

// TODO : Replace this with AddChaseTarget(AActor* NewChaseTarget) that puts multiple chase targets into an array.
void USimulationSubsystem::SetChaseTarget(AActor* NewChaseTarget)
{
	ChaseTarget = NewChaseTarget;
}

FTransform USimulationSubsystem::GetTransform(const uint32 AgentIndex) const
{
	if(!AgentTransforms.IsValidIndex(AgentIndex))
	{
		return FTransform::Identity;
	}

	return AgentTransforms[AgentIndex];
}

void USimulationSubsystem::UpdateTransform(const uint32 AgentIndex)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USimulationSubsystem::UpdateTransform)
	const FRotator& Rotator = UKismetMathLibrary::MakeRotFromX(AgentsData[AgentIndex]->Velocity.GetSafeNormal());
	const FVector& Location = AgentsData[AgentIndex]->Location;
	
	AgentTransforms[AgentIndex] = FTransform(Rotator + Configuration->RotationOffset, Location);
}

const TArray<FTransform>& USimulationSubsystem::GetTransforms() const
{
	return AgentTransforms;
}

void USimulationSubsystem::RunSimulationLogicOnSingleAgent(const uint32 AgentIndex) const
{
	SenseNearbyAgents(AgentIndex);
	ApplyBehaviourOnAgent(AgentIndex);
}

void USimulationSubsystem::ApplyBehaviourOnAgent(const uint32 AgentIndex) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USimulationSubsystem::ApplyBehaviourOnAgent)
	UAgent* TargetAgent = AgentsData[AgentIndex];

	FVector MovementForce = FVector::ZeroVector;
	if (ShouldAgentFlock(AgentIndex))
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
		{
			if (const ISeekingInterface* SeekingInterface = Cast<ISeekingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += SeekingInterface->CalculateSeekForce(TargetAgent, ChaseTarget, Configuration->AgentsMaxSpeed);
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

void USimulationSubsystem::SenseNearbyAgents(const uint32 AgentIndex) const
{
	UAgent* TargetAgent = AgentsData[AgentIndex];

	TargetAgent->NumNearbyAgents = 0;
	for(int32& Index : TargetAgent->NearbyAgentIndices)
	{
		Index = -1;
	}
	ImplicitGrid.Search(TargetAgent->Location, Configuration->AgentSenseRange, TargetAgent->NearbyAgentIndices, TargetAgent->NumNearbyAgents);
}

void USimulationSubsystem::UpdateAgent(const uint32 AgentIndex, const float DeltaSeconds)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USimulationSubsystem::UpdateAgent)
	UAgent* TargetAgent = AgentsData[AgentIndex];
	TargetAgent->UpdateState(DeltaSeconds);
}

bool USimulationSubsystem::ShouldAgentFlock(const uint32 AgentIndex) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USimulationSubsystem::ShouldAgentFlock)
	if (Configuration->bForceLeadership)
	{
		return true;
	}

	UAgent* TargetAgent = AgentsData[AgentIndex];

	const float LeadershipCheck_MaximumValue = Configuration->LeaderCheckParameters.SearchRadius.GetUpperBoundValue();
	const float LeadershipCheck_MinimumValue = Configuration->LeaderCheckParameters.SearchRadius.GetLowerBoundValue();
	const float HalfFOV = Configuration->LeaderCheckParameters.FOVHalfAngle;

	uint32 NumAgentsFound = 0;
	for (const int32 OtherAgentIndex : TargetAgent->NearbyAgentIndices)
	{
		if(OtherAgentIndex == -1)
		{
			break;
		}
		
		const UAgent* OtherAgent = AgentsData[OtherAgentIndex];
		if (Utility::IsPointInFOV(
			TargetAgent->Location, TargetAgent->GetForwardVector(), OtherAgent->Location,
			LeadershipCheck_MinimumValue, LeadershipCheck_MaximumValue, HalfFOV))
		{
			++NumAgentsFound;
		}
	}

	// If this Agent cannot see any other agents in its view cone, it cannot flock other agents and should rather chase a target.
	return NumAgentsFound == 0;
}
