// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SimulationSubsystem.h"
#include "Subsystems/SpatialGridSubsystem.h"

#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"

#include "Common/Utility.h"
#include "Kismet/KismetMathLibrary.h"

#include "Configuration/SimulatorConfiguration.h"
#include "Kismet/GameplayStatics.h"
#include "Level/AgentsLevelBase.h"

USimulationSubsystem::~USimulationSubsystem()
{
	UE_LOG(LogTemp, Warning, TEXT("Simulation Subsystem : Destructor"));
}

// TODO: This is a temporary patch to a problem where we can't prevent CDOs from serializing certain variables.
// This function shouldn't exist at all.
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

void USimulationSubsystem::Init(USimulatorConfiguration* NewConfiguration, AAgentsLevelBase* LevelActor)
{
	checkf(NewConfiguration != nullptr, TEXT("Simulation Configuration cannot be null."));

	Configuration = NewConfiguration;
	SpatialGrid = GetGameInstance()->GetSubsystem<USpatialGridSubsystem>();

	AgentsLevelBase = LevelActor;
	ResetInfluences();
}

void USimulationSubsystem::StartSimulation()
{
	checkf(Configuration != nullptr, TEXT("Simulation Configuration is null."));
	//LaunchThreads();
	//LaunchAsyncOperations();
}

void USimulationSubsystem::Tick(const float DeltaTime)
{
	ParallelFor(AgentsData.Num(),
				[this, DeltaTime](const int32 Index)
				{
					RunSimulationLogicOnSingleAgent(Index);
					UpdateAgent(Index, DeltaTime);
				},
				EParallelForFlags::BackgroundPriority);
}

UAgent* USimulationSubsystem::CreateAgent(const FVector& InitialLocation, const FVector& InitialVelocity)
{
	AgentsData.Add(NewObject<UAgent>());
	AgentTransforms.Add(FTransform::Identity);
	
	AgentsData.Last()->Location = InitialLocation;
	AgentsData.Last()->Velocity = InitialVelocity;

	return AgentsData.Last();
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
	const FRotator& Rotator = UKismetMathLibrary::MakeRotFromX(AgentsData[AgentIndex]->Velocity.GetSafeNormal());
	const FVector& Location = AgentsData[AgentIndex]->Location;
	
	AgentTransforms[AgentIndex] = FTransform(Rotator + Configuration->RotationOffset, Location);
}

const TArray<FTransform>& USimulationSubsystem::GetTransforms() const
{
	return AgentTransforms;
}

void USimulationSubsystem::LaunchThreads()
{
	int LowerLimit = 0;
	const uint32 BucketSize = FMath::CeilToInt32(AgentsData.Num() / static_cast<float>(Configuration->ThreadCount));

	for (uint32 ThreadIndex = 0; ThreadIndex < Configuration->ThreadCount; ++ThreadIndex)
	{
		const int UpperLimit = FMath::Min(AgentsData.Num() - 1, static_cast<int>(LowerLimit + BucketSize - 1));
		const FString& ThreadName = "SimulationThread" + ThreadIndex;
		FRunData Data(LowerLimit, UpperLimit,
		              FRunData::FRunnableCallback::CreateUObject(
			              this, &USimulationSubsystem::RunSimulationLogicOnSingleAgent), ThreadName);

		Runnables.Add(MakeUnique<FSimulationRunnable>(Data));
		UE_LOG(LogTemp, Warning, TEXT("Created New Thread : Bucket [%d, %d]"), LowerLimit, UpperLimit);

		LowerLimit += BucketSize;
	}
}

void USimulationSubsystem::LaunchAsyncOperations()
{
	AsyncResults.Reset();

	uint32 LowerLimit = 0;
	const uint32 BucketSize = FMath::CeilToInt32(AgentsData.Num() / static_cast<float>(Configuration->ThreadCount));

	for (uint32 ThreadIndex = 0; ThreadIndex < Configuration->ThreadCount; ++ThreadIndex)
	{
		uint32 UpperLimit = FMath::Min(AgentsData.Num() - 1, static_cast<int>(LowerLimit + BucketSize - 1));
		AsyncResults.Add
		(
			Async
			(
				EAsyncExecution::Thread,
				[this, LowerLimit, UpperLimit]()
				{
					RunAsyncLogic(LowerLimit, UpperLimit);
				}
			)
		);

		LowerLimit += BucketSize;
	}
}

void USimulationSubsystem::RunAsyncLogic(const uint32 LowerLimit, const uint32 UpperLimit) const
{
	while (!bStopRequested)
	{
		for (uint32 AgentIndex = LowerLimit; AgentIndex <= UpperLimit; ++AgentIndex)
		{
			RunSimulationLogicOnSingleAgent(AgentIndex);
		}
	}
}

void USimulationSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Warning, TEXT("Simulation Subsystem : BeginDestroy"));
	bStopRequested = true;

	for (const TFuture<void>& Result : AsyncResults)
	{
		Result.Wait();
	}
}

void USimulationSubsystem::RunSimulationLogicOnSingleAgent(const uint32 AgentIndex) const
{
	SenseNearbyAgents(AgentIndex);
	ApplyBehaviourOnAgent(AgentIndex);
}

void USimulationSubsystem::ApplyBehaviourOnAgent(const uint32 AgentIndex) const
{
	UAgent* TargetAgent = AgentsData[AgentIndex];

	FVector MovementForce = FVector::ZeroVector;
	if (ShouldAgentFlock(AgentIndex))
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->ChaseBehaviors)
		{
			if (const ISeekingInterface* SeekingInterface = Cast<ISeekingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += SeekingInterface->CalculateSeekForce(TargetAgent, ChaseTarget,
				                                                      Configuration->AgentsMaxSpeed);
			}
		}
	}
	else
	{
		for (const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : Configuration->FlockBehaviors)
		{
			if (const IFlockingInterface* FlockingInterface = Cast<IFlockingInterface>(Behaviour->GetDefaultObject()))
			{
				MovementForce += FlockingInterface->CalculateSteerForce(
					TargetAgent, AgentsData, Configuration->AgentsMaxSpeed);
			}
		}
	}

	TargetAgent->MovementForce = MovementForce;
}

void USimulationSubsystem::SenseNearbyAgents(const uint32 AgentIndex) const
{
	UAgent* TargetAgent = AgentsData[AgentIndex];
	TargetAgent->NearbyAgentIndices.Reset();
	if (SpatialGrid)
	{
		SpatialGrid->SearchActors(TargetAgent->Location, Configuration->AgentSenseRange,
		                          TargetAgent->NearbyAgentIndices);
	}
}

void USimulationSubsystem::UpdateAgent(const uint32 AgentIndex, const float DeltaSeconds)
{
	UAgent* TargetAgent = AgentsData[AgentIndex];
	TargetAgent->UpdateState(DeltaSeconds);
	UpdateTransform(AgentIndex);
}

void USimulationSubsystem::FixedUpdateAgent(const uint32 AgentIndex)
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
