// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SimulationSubsystem.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"
#include "Common/SimulationSettings.h"
#include "Common/Utility.h"
#include "Kismet/KismetMathLibrary.h"
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

UAgentData* USimulationSubsystem::CreateAgent(const FVector& InitialLocation, const FVector& InitialVelocity)
{
	const int32 Index = AgentsData.AddUnique(NewObject<UAgentData>());
	AgentsData[Index]->Location = InitialLocation;
	AgentsData[Index]->Velocity = InitialVelocity;
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
		const int UpperLimit = FMath::Min(AgentsData.Num() - 1, static_cast<int>(LowerLimit + BucketSize - 1));
		const FString& ThreadName = "SimulationThread" + ThreadIndex;
		FRunData Data(LowerLimit, UpperLimit,
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::SenseNearbyAgents),
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::ApplyBehaviourOnAgent),
		              FRunData::FRunnableCallback::CreateUObject(this, &USimulationSubsystem::FixedUpdateAgent),
		              ThreadName);

		Runnables.Add(MakeUnique<FSimulationRunnable>(Data));
		UE_LOG(LogTemp, Log, TEXT("Created New Thread : Bucket [%d, %d]"), LowerLimit, UpperLimit);

		LowerLimit += BucketSize;
	}
}

void USimulationSubsystem::Update(float DeltaTime)
{
	for(int AgentIndex = 0; AgentIndex < AgentsData.Num(); ++AgentIndex)
	{
		UpdateAgent(AgentIndex, DeltaTime);
	}
}

void USimulationSubsystem::StartSimulation()
{
	LaunchThreads();
}

int USimulationSubsystem::GetNumAgents() const
{
	return AgentsData.Num();
}

FTransform USimulationSubsystem::GetTransform(uint32 Index, const FRotator& RotationOffset) const
{
	if(!AgentsData.IsValidIndex(Index))
	{
		return FTransform::Identity;
	}

	const FRotator& Rotator = RotationOffset + UKismetMathLibrary::MakeRotFromX(AgentsData[Index]->Velocity.GetSafeNormal());
	const FVector& Location = AgentsData[Index]->Location;

	return FTransform(Rotator, Location);
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

void USimulationSubsystem::UpdateAgent(const uint32 Index, const float DeltaTime)
{
	if(!AgentsData.IsValidIndex(Index))
	{
		return;
	}

	UAgentData* TargetAgent = AgentsData[Index];
	TargetAgent->UpdateState(DeltaTime);
}

void USimulationSubsystem::FixedUpdateAgent(const uint32 Index)
{
	UpdateAgent(Index, SimulationSettings->FixedDeltaTime);
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
