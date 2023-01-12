// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AutonomousMovementComponent.h"
#include "Common/Utility.h"
#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "Behaviours/Base/SeekingInterface.h"
#include "Components/SphereComponent.h"
#include "Subsystems/SpatialGridSubsystem.h"

UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAutonomousMovementComponent::InitializeSphereComponent()
{
	SphereComponent = GetOwner()->FindComponentByClass<USphereComponent>();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &UAutonomousMovementComponent::OnEnterDetection);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &UAutonomousMovementComponent::OnExitDetection);
}

void UAutonomousMovementComponent::ResetBehaviours()
{
	for(const auto& Behaviour : FlockingBehaviours)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour->GetDefaultObject())->ResetInfluence();
	}

	for(const auto& Behaviour : SeekingBehaviours)
	{
		Cast<UBaseAutonomousBehaviour>(Behaviour->GetDefaultObject())->ResetInfluence();
	}
}

void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PreviousLocation = GetOwner()->GetActorLocation();
	
	InitializeSphereComponent();
	ResetBehaviours();

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if(GameInstance != nullptr)
	{
		GridSubsystem = GameInstance->GetSubsystem<USpatialGridSubsystem>();
		if(GridSubsystem.IsValid())
		{
			GridSubsystem->OnActorPresenceUpdatedEvent.AddDynamic(this, &UAutonomousMovementComponent::HandleActorPresenceUpdated);
		}
	}
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

void UAutonomousMovementComponent::DebugOtherActors()
{
	if(bDebugSense)
	{
		if(GridSubsystem.IsValid())
		{
			TArray<int32> SensedActorIndices;
			GridSubsystem->GetActorsInArea(GetOwner()->GetActorLocation(), DebugSenseRange, SensedActorIndices);
			
			for(const int Index : SensedActorIndices)
			{
				if(!AllAgents.IsValidIndex(Index)) continue;
				
				const AActor* OtherActor = AllAgents[Index];
				if(OtherActor == nullptr || OtherActor == GetOwner()) continue;
				
				DrawDebugBox(GetWorld(), OtherActor->GetActorLocation(), FVector::One() * DebugBoxSize, FColor::Red, false, 0.05f);
			}
		}
	}
}

void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
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

			MovementForce += FlockingBehaviour->CalculateSteerForce(GetOwner(), SensedAgents, MaxSpeed);
		}
		SetIsFollowing.Broadcast();
	}

	DebugOtherActors();
	
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
	for(const TWeakObjectPtr<AActor>& Agent : SensedAgents)
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

void UAutonomousMovementComponent::OnEnterDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsValid(OtherActor) && OtherActor->Tags.Contains(AgentsTag))
	{
		if(!SensedAgents.Contains(OtherActor))
		{
			SensedAgents.Add(OtherActor);
		}
	}
}

void UAutonomousMovementComponent::OnExitDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(SensedAgents.Contains(OtherActor))
	{
		SensedAgents.Remove(OtherActor);
	}
}
