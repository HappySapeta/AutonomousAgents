// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AutonomousMovementComponent.h"
#include "Common/Utility.h"
#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"
#include "Subsystems/SpatialGridSubsystem.h"

UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAutonomousMovementComponent::ResetBehaviours()
{
	// TODO: Remove redundant calls.
	for(const auto& Behaviour : FlockingBehaviours)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour->GetDefaultObject())->ResetInfluence();
	}

	for(const auto& Behaviour : SeekingBehaviours)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour->GetDefaultObject())->ResetInfluence();
	}
}

void UAutonomousMovementComponent::BindEventToGridSubsystem()
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if(GameInstance != nullptr)
	{
		GridSubsystem = GameInstance->GetSubsystem<USpatialGridSubsystem>();
		//if(GridSubsystem.IsValid())
		//{
		//	GridSubsystem->OnActorPresenceUpdatedEvent.AddDynamic(this, &UAutonomousMovementComponent::HandleActorPresenceUpdated);
		//	GridSubsystem->GetAllActors(AllAgents);
		//}
	}
}

void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PreviousLocation = GetOwner()->GetActorLocation();
	ResetBehaviours();
	BindEventToGridSubsystem();
}

void UAutonomousMovementComponent::HandleActorPresenceUpdated(AActor* Actor)
{
	if(Actor != nullptr)
	{
		if(AllAgents.Contains(Actor))
		{
			AllAgents.Remove(Actor);
		}
		else
		{
			AllAgents.Add(Actor);
		}
	}
}

void UAutonomousMovementComponent::SenseNearbyAgents()
{
	if(AllAgents.Num() == 0)
	{
		GridSubsystem->GetAllActors(AllAgents);
	}
	else
	{
		return;
	}
	
	NearbyAgents.Reset();
	if(!GridSubsystem.IsValid()) return;

	TArray<int> NearbyAgentIndices;
	GridSubsystem->GetActorIndicesInRegion(GetOwner()->GetActorLocation(), AgentSenseRange, NearbyAgentIndices);
	
	for(const int Index : NearbyAgentIndices)
	{
		if(AllAgents[Index] == GetOwner()) continue;
		NearbyAgents.Add(AllAgents[Index]);
	}
}

void UAutonomousMovementComponent::InvokeBehaviours()
{
	SenseNearbyAgents();

	if(bForceLeadership || CanAgentLead())
	{
		for(const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : SeekingBehaviours)
		{
			const ISeekingInterface* SeekingBehaviour = Cast<ISeekingInterface>(Behaviour->GetDefaultObject());
			if(!SeekingBehaviour) continue;

			MovementForce += SeekingBehaviour->CalculateSeekForce(GetOwner(), ChaseTarget, MaxSpeed);
		}
		SetIsChasing.Broadcast();
	}
	else
	{
		for(const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : FlockingBehaviours)
		{
			const IFlockingInterface* FlockingBehaviour = Cast<IFlockingInterface>(Behaviour->GetDefaultObject());
			if(!FlockingBehaviour) continue;

			MovementForce += FlockingBehaviour->CalculateSteerForce(GetOwner(), NearbyAgents, MaxSpeed);
		}
		SetIsFollowing.Broadcast();
	}
}

void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	InvokeBehaviours();
	PhysicsUpdate(DeltaTime);
}

void UAutonomousMovementComponent::PhysicsUpdate(float DeltaTime)
{
	const FVector& NewVelocity = PreviousVelocity + MovementForce * DeltaTime;
	const FVector& NewLocation = PreviousLocation + NewVelocity * DeltaTime;
	GetOwner()->SetActorLocation(NewLocation);
	
	MovementForce = FVector::ZeroVector;

	// GetOwner()->GetVelocity() is stale compared to NewVelocity. So previous velocity is assigned to be equal to NewVelocity.
	PreviousVelocity = NewVelocity;
	PreviousLocation = NewLocation;
}

void UAutonomousMovementComponent::GetAgentsInView(float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle, FActorArray& AgentsInView) const
{
	AgentsInView.Reset();
	for(const TWeakObjectPtr<AActor>& Agent : AllAgents)
	{
		if(Agent.IsValid() && Utility::IsPointInFOV(
			GetOwner()->GetActorLocation(), GetOwner()->GetActorForwardVector(),Agent->GetActorLocation(),
			MinimumSearchRadius, MaximumSearchRadius, FOVHalfAngle))
		{
			AgentsInView.Add(Agent);
		}
	}
}

bool UAutonomousMovementComponent::CanAgentLead() const
{
	FActorArray OtherAgents;
	GetAgentsInView(
		LeaderSearchParameters.SearchRadius.GetLowerBoundValue(),
		LeaderSearchParameters.SearchRadius.GetUpperBoundValue(), LeaderSearchParameters.FOVHalfAngle, OtherAgents);

	return OtherAgents.Num() == 0;
}

void UAutonomousMovementComponent::SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget)
{
	if(NewTarget.IsValid() && ChaseTarget != NewTarget)
	{
		ChaseTarget = NewTarget;
	}
}