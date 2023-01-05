// Fill out your copyright notice in the Description page of Project Settings.

#include "AutonomousMovementComponent.h"

#include "Components/SphereComponent.h"

UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PreviousLocation = GetOwner()->GetActorLocation();
	SphereComponent = GetOwner()->FindComponentByClass<USphereComponent>();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &UAutonomousMovementComponent::OnEnterDetection);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &UAutonomousMovementComponent::OnExitDetection);
}

void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(IsAgentLonely())
	{
		SetIsChasing.Broadcast();
		PerformChaseTarget();
	}
	else
	{
		SetIsFollowing.Broadcast();
		PerformFlockCohesion();
		PerformFlockSeparation();
	}
	
	UpdateActorLocation(DeltaTime);
}

void UAutonomousMovementComponent::PerformChaseTarget()
{
	if(!ChaseTarget.IsValid()) return;

	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - GetOwner()->GetActorLocation();
	
	DesiredVelocity.Normalize();
	DesiredVelocity *= MaxSpeed;

	const FVector& ChaseManeuver = DesiredVelocity - PreviousVelocity;
	
	AddForce(ChaseManeuver * ChaseForce);

	if(FlockSearchConfig.bDebug)
	{
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), ChaseTarget->GetActorLocation(), FColor::Emerald, false, 0.01f, 0, 5.0f);
	}
}

void UAutonomousMovementComponent::PerformFlockCohesion()
{
	if(!bCohesionEnabled) return;
	
	TArray<TWeakObjectPtr<AActor>> OtherAgents;
	GetAgentsInView(CohesionConfig.MinimumSearchRadius, CohesionConfig.MaximumSearchRadius, CohesionConfig.FOVHalfAngle, OtherAgents);

	FVector HerdLocation = FVector::ZeroVector;
	
	if(OtherAgents.Num() > 0)
	{
		for(const TWeakObjectPtr<AActor>& OtherAgent : OtherAgents)
		{
			const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
			HerdLocation += OtherAgentLocation;
			
			if(CohesionConfig.bDebug)
			{
				DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), OtherAgentLocation, FColor::Blue, false, 0.02f, 0, 5.0f);
			}
		}

		HerdLocation /= OtherAgents.Num();
		const FVector& DesiredVelocity = (HerdLocation - GetOwner()->GetActorLocation()).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - PreviousVelocity;
		AddForce(CohesionManeuver * CohesionForce);
	}
}

void UAutonomousMovementComponent::PerformFlockSeparation()
{
	if(!bSeparationEnabled)
	{
		return;
	}

	TArray<TWeakObjectPtr<AActor>> OtherAgents;
	GetAgentsInView(SeparationConfig.MinimumSearchRadius, SeparationConfig.MaximumSearchRadius , SeparationConfig.FOVHalfAngle, OtherAgents);
	
	const int NumAvoidableAgents = OtherAgents.Num();
	
	FVector AvoidanceVector = FVector::ZeroVector;

	if(NumAvoidableAgents > 0)
	{
		for(const TWeakObjectPtr<AActor>& OtherAgent : OtherAgents)
		{
			const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
			FVector OtherAgentVector = GetOwner()->GetActorLocation() - OtherAgentLocation;
			const float OtherAgentDistance = OtherAgentVector.Length();
			
			OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
			AvoidanceVector += OtherAgentVector;
			
			if(SeparationConfig.bDebug)
			{
				DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), OtherAgentLocation, FColor::Red, false, 0.02f, 0, 5.0f);
			}
		}

		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - PreviousVelocity;
		AddForce(SeparationManeuver * SeparationForce);
	}
}

void UAutonomousMovementComponent::AddForce(const FVector& Force)
{
	MovementForce += Force;
}

void UAutonomousMovementComponent::UpdateActorLocation(float DeltaTime)
{
	if(bLimitForce && MovementForce.Length() > MaxForce)
	{
		MovementForce = MovementForce.GetSafeNormal() * MaxForce;
	}
	
	const FVector& NewVelocity = PreviousVelocity + MovementForce * DeltaTime;
	const FVector& NewLocation = PreviousLocation + NewVelocity * DeltaTime;
	GetOwner()->SetActorLocation(NewLocation);
	
	MovementForce = FVector::ZeroVector;
	PreviousVelocity = NewVelocity;
	PreviousLocation = NewLocation;
}

void UAutonomousMovementComponent::GetAgentsInView(float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle, TArray<TWeakObjectPtr<AActor>>& AgentsInView) const
{
	AgentsInView.Reset();
	for(const TWeakObjectPtr<AActor>& Agent : SensedAgents)
	{
		if(Agent.IsValid() && IsAgentInSpecifiedViewCone(Agent->GetActorLocation(), MinimumSearchRadius, MaximumSearchRadius, FOVHalfAngle))
		{
			AgentsInView.Add(Agent);
		}
	}
}

bool UAutonomousMovementComponent::IsAgentLonely() const
{
	TArray<TWeakObjectPtr<AActor>> OtherAgents;
	GetAgentsInView(FlockSearchConfig.MinimumSearchRadius, FlockSearchConfig.MaximumSearchRadius, FlockSearchConfig.FOVHalfAngle, OtherAgents);

	return OtherAgents.Num() == 0;
}

bool UAutonomousMovementComponent::IsAgentInSpecifiedViewCone(const FVector& OtherAgentLocation, float MinimumSearchRadius, float MaximumSearchRadius, float HalfFOV) const
{
	const FVector& OtherAgentVector = OtherAgentLocation - GetOwner()->GetActorLocation();

	const FVector& Forward = GetOwner()->GetActorForwardVector();
	const float DotProduct = OtherAgentVector.GetSafeNormal().Dot(Forward); 

	const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(DotProduct)));
	const float Distance = OtherAgentVector.Length();
	
	return Distance > MinimumSearchRadius && Distance < MaximumSearchRadius && Angle < HalfFOV;
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
