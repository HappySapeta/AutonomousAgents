// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SimulationSubsystem.h"
#include "Subsystems/SpatialGridSubsystem.h"

#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"

#include "Common/Utility.h"
#include "Kismet/KismetMathLibrary.h"

#include "Configuration/SimulatorConfiguration.h"

void USimulationSubsystem::ResetInfluences()
{
	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
	{
		// CDO always serialize influence parameters, it is important to reset them before starting the simulation.
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}

	for(TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
	{
		// CDO always serialize influence parameters, it is important to reset them before starting the simulation.
		Cast<UBaseAutonomousBehaviour>(Behaviour.GetDefaultObject())->ResetInfluence();
	}
}

void USimulationSubsystem::Init(USimulatorConfiguration* NewConfiguration)
{
	checkf(NewConfiguration != nullptr, TEXT("Simulation Configuration cannot be null."));
	
	Configuration = NewConfiguration;
	SpatialGrid = GetGameInstance()->GetSubsystem<USpatialGridSubsystem>();
	
	ResetInfluences();
}

void USimulationSubsystem::StartSimulation()
{
	checkf(Configuration != nullptr, TEXT("Simulation Configuration is null."));
	StartSimulation_Internal();
}

UAgent* USimulationSubsystem::CreateAgent(const FVector& InitialLocation, const FVector& InitialVelocity)
{
	AgentsData.Add(NewObject<UAgent>());
	AgentsData.Last()->Location = InitialLocation;
	AgentsData.Last()->Velocity = InitialVelocity;
	return AgentsData.Last();
}

// TODO : Replace this with AddChaseTarget(AActor* NewChaseTarget) that puts multiple chase targets into an array.
void USimulationSubsystem::SetChaseTarget(AActor* NewChaseTarget)
{
	ChaseTarget = NewChaseTarget;
}

FTransform USimulationSubsystem::GetTransform(uint32 AgentIndex, const FRotator& RotationOffset) const
{
	const FRotator& Rotator = UKismetMathLibrary::MakeRotFromX(AgentsData[AgentIndex]->Velocity.GetSafeNormal());
	const FVector& Location = AgentsData[AgentIndex]->Location;

	const FTransform& Transform = FTransform(Rotator + RotationOffset, Location);
	return Transform;
}

// TODO : Implement an alternative solution so that updates sync well with threaded operations.
void USimulationSubsystem::Tick(float DeltaSeconds) const
{
	checkf(Configuration != nullptr, TEXT("Simulation Configuration cannot be null."));
	for(int AgentIndex = 0; AgentIndex < AgentsData.Num(); ++AgentIndex)
	{
		UpdateAgent(AgentIndex, DeltaSeconds);
	}
}

void USimulationSubsystem::StartSimulation_Internal()
{
	int LowerLimit = 0;
	const uint32 BucketSize = FMath::CeilToInt32(AgentsData.Num() / static_cast<float>(Configuration->ThreadCount));
	
	for(uint32 ThreadIndex = 0; ThreadIndex < Configuration->ThreadCount; ++ThreadIndex)
	{
		const int UpperLimit = FMath::Min(AgentsData.Num() - 1, static_cast<int>(LowerLimit + BucketSize - 1));
		const FString& ThreadName = "SimulationThread" + ThreadIndex;
		FRunData Data(LowerLimit, UpperLimit,
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::SimulationLogic),
		              ThreadName);

		Runnables.Add(MakeUnique<FSimulationRunnable>(Data));
		UE_LOG(LogTemp, Warning, TEXT("Created New Thread : Bucket [%d, %d]"), LowerLimit, UpperLimit);

		LowerLimit += BucketSize;
	}
}

void USimulationSubsystem::SimulationLogic(const uint32 AgentIndex) const
{
	SenseNearbyAgents(AgentIndex);
	ApplyBehaviourOnAgent(AgentIndex);
}

void USimulationSubsystem::ApplyBehaviourOnAgent(const uint32 AgentIndex) const
{
	if(!AgentsData.IsValidIndex(AgentIndex))
	{
		return;
	}

	UAgent* TargetAgent = AgentsData[AgentIndex];
	
	FVector MovementForce = FVector::ZeroVector;
	if(ShouldAgentFlock(AgentIndex))
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

void USimulationSubsystem::SenseNearbyAgents(const uint32 AgentIndex) const
{
	if(!AgentsData.IsValidIndex(AgentIndex))
	{
		return;
	}

	UAgent* TargetAgent = AgentsData[AgentIndex];
	TargetAgent->NearbyAgentIndices.Reset();
	if (SpatialGrid)
	{
		SpatialGrid->SearchActors(TargetAgent->Location, Configuration->AgentSenseRange,TargetAgent->NearbyAgentIndices);
	}
}

void USimulationSubsystem::UpdateAgent(const uint32 AgentIndex, const float DeltaSeconds) const
{
	UAgent* TargetAgent = AgentsData[AgentIndex];
	TargetAgent->UpdateState(DeltaSeconds);
}

void USimulationSubsystem::FixedUpdateAgent(const uint32 AgentIndex) const
{
	UpdateAgent(AgentIndex, Configuration->FixedDeltaTime);
}

bool USimulationSubsystem::ShouldAgentFlock(const uint32 AgentIndex) const
{
	if (Configuration->bForceLeadership)
	{
		return true;
	}

	UAgent* TargetAgent = AgentsData[AgentIndex];

	const float LeadershipCheck_MaximumValue = Configuration->LeaderCheckParameters.SearchRadius.GetUpperBoundValue();
	const float LeadershipCheck_MinimumValue = Configuration->LeaderCheckParameters.SearchRadius.GetLowerBoundValue();
	const float HalfFOV = Configuration->LeaderCheckParameters.FOVHalfAngle;
	
	uint32 NumAgentsFound = 0;
	for (const uint32 OtherAgentIndex : TargetAgent->NearbyAgentIndices)
	{
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
