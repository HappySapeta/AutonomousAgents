
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

void UAutonomousMovementComponent::FetchGridSubsystem()
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if(GameInstance != nullptr)
	{
		GridSubsystem = GameInstance->GetSubsystem<USpatialGridSubsystem>();
	}
}

void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PreviousLocation = GetOwner()->GetActorLocation();
	ResetBehaviours();
	FetchGridSubsystem();
}

void UAutonomousMovementComponent::SenseNearbyAgents()
{
	NearbyAgentIndices.Reset();
	if(GridSubsystem.IsValid())
	{
		GridSubsystem->SearchActors(GetOwner()->GetActorLocation(), AgentSenseRange, NearbyAgentIndices);
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
	}
	else
	{
		if(!GridSubsystem.IsValid())
		{
			return;
		}
		
		for(const TSubclassOf<UBaseAutonomousBehaviour>& Behaviour : FlockingBehaviours)
		{
			const IFlockingInterface* FlockingBehaviour = Cast<IFlockingInterface>(Behaviour->GetDefaultObject());
			if(!FlockingBehaviour) continue;

			MovementForce += FlockingBehaviour->CalculateSteerForce(GetOwner(), GridSubsystem->GetActorArray(), NearbyAgentIndices, MaxSpeed);
		}
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

bool UAutonomousMovementComponent::CanAgentLead() const
{
	if(!GridSubsystem.IsValid()) return true;

	int NumAgentsInView = 0;
	const FActorArray* AllActors = GridSubsystem->GetActorArray();
	for(const uint32 Index : NearbyAgentIndices)
	{
		const FWeakActorPtr& Agent = AllActors->operator[](Index);
		
		if(Agent.IsValid() && Utility::IsPointInFOV(
			GetOwner()->GetActorLocation(), GetOwner()->GetActorForwardVector(),
			Agent->GetActorLocation(),
			LeaderSearchParameters.SearchRadius.GetLowerBoundValue(),
			LeaderSearchParameters.SearchRadius.GetUpperBoundValue(),
			LeaderSearchParameters.FOVHalfAngle))
		{
			++NumAgentsInView;
		}
	}
	
	return NumAgentsInView == 0;
}

void UAutonomousMovementComponent::SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget)
{
	if(NewTarget.IsValid() && ChaseTarget != NewTarget)
	{
		ChaseTarget = NewTarget;
	}
}