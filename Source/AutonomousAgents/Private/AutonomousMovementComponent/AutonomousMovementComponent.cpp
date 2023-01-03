// Fill out your copyright notice in the Description page of Project Settings.

#include "AutonomousMovementComponent.h"

#include <AgentPawn/AgentPawn.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Perception/AIPerceptionComponent.h>

// Sets default values for this component's properties
UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = Cast<UFloatingPawnMovement>(GetOwner()->FindComponentByClass(UFloatingPawnMovement::StaticClass()));

	if(const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if(const AController* Controller = Pawn->GetController())
		{
			PerceptionComponent = Cast<UAIPerceptionComponent>(Controller->FindComponentByClass(UAIPerceptionComponent::StaticClass()));
		}
	}

	MovementInput = FVector::ZeroVector;
}

// Called every frame
void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SenseOtherAgents();
	
	if(CanAgentBecomeSeeker())
	{
		bIsSeeker = true;
		PerformChase();
	}
	else
	{
		bIsSeeker = false;
		ApplyCohesion();
		ApplySeparation();
	}

	ApplyInput();
}

void UAutonomousMovementComponent::PerformChase()
{
	if(!ChaseTarget.IsValid()) return;
	if(!MovementComponent.IsValid()) return;

	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - MovementComponent->GetActorLocation();
	DesiredVelocity.Normalize();
	DesiredVelocity *= MovementComponent->GetMaxSpeed();

	const FVector& SteerInput = DesiredVelocity - MovementComponent->Velocity;

	MovementInput += SteerInput;
}

void UAutonomousMovementComponent::ApplyCohesion()
{
	if(!MovementComponent.IsValid()) return;
	
	uint32 NumCohesiveAgents = 0;
	FVector HerdLocation = FVector::ZeroVector;
	FVector HerdVelocity = FVector::ZeroVector;

	for(const AActor* OtherAgent : NearbyAgents)
	{
		if(Cast<AAgentPawn>(OtherAgent) == nullptr)
		{
			continue;
		}

		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
		HerdLocation += OtherAgentLocation;
		HerdVelocity += OtherAgent->GetVelocity();
		++NumCohesiveAgents;

		if(bDebugCohesion)
		{
			DrawDebugLine(GetWorld(), MovementComponent->GetActorLocation(), OtherAgentLocation, FColor::Blue, false, 0.02f, 0, 5.0f);
		}
	}

	if(NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - MovementComponent->GetActorLocation()).GetSafeNormal() * MovementComponent->GetMaxSpeed();
		const FVector& CohesionManeuver = DesiredVelocity - MovementComponent->Velocity;
		MovementInput += CohesionManeuver * CohesionBias;

		HerdVelocity /= NumCohesiveAgents;
		const FVector& AlignmentManeuver = HerdVelocity.GetSafeNormal() * MovementComponent->MaxSpeed - MovementComponent->Velocity;
		MovementInput += AlignmentManeuver * AlignmentBias;
	}
}

void UAutonomousMovementComponent::ApplySeparation()
{
	if(!MovementComponent.IsValid()) return;
	
	uint32 NumAvoidableAgents = 0;
	FVector AvoidanceVector = FVector::ZeroVector;
	
	for(const AActor* OtherAgent : NearbyAgents)
	{
		if(Cast<AAgentPawn>(OtherAgent) == nullptr)
		{
			continue;
		}

		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
		if(IsAgentInSpecifiedViewCone(OtherAgentLocation, SeparationSightRadius, SeparationSightHalfFOV))
		{
			FVector OtherAgentVector = MovementComponent->GetActorLocation() - OtherAgentLocation;
			const float OtherAgentDistance = OtherAgentVector.Length();
			OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
			AvoidanceVector += OtherAgentVector;
			
			++NumAvoidableAgents;

			if(bDebugSeparation)
			{
				DrawDebugLine(GetWorld(), MovementComponent->GetActorLocation(), OtherAgentLocation, FColor::Red, false, 0.02f, 0, 5.0f);
			}
		}
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MovementComponent->GetMaxSpeed();

		const FVector& SeparationManeuver = AvoidanceVector - MovementComponent->Velocity;
		MovementInput += SeparationManeuver * SeparationBias;
	}
}

void UAutonomousMovementComponent::SenseOtherAgents()
{
	if (PerceptionComponent.IsValid())
	{
		NearbyAgents.Reset();
		//PerceptionComponent->RequestStimuliListenerUpdate();
		PerceptionComponent->GetCurrentlyPerceivedActors(PerceptionComponent->GetDominantSense(), NearbyAgents);
	}
}

bool UAutonomousMovementComponent::CanAgentBecomeSeeker() const
{
	return NearbyAgents.Num() == 0;
}

bool UAutonomousMovementComponent::IsAgentInSpecifiedViewCone(const FVector& OtherAgentLocation, float Radius, float HalfFOV) const
{
	const FVector& OtherAgentVector = OtherAgentLocation - MovementComponent->GetActorLocation();

	const FVector& Forward = GetOwner()->GetActorForwardVector();
	const float DotProduct = OtherAgentVector.GetSafeNormal().Dot(Forward); 
	const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(DotProduct)));
	return OtherAgentVector.Length() < Radius && Angle < HalfFOV;
}

void UAutonomousMovementComponent::ApplyInput()
{
	if(MovementComponent.IsValid())
	{
		MovementComponent->AddInputVector(MovementInput.GetSafeNormal());
		MovementInput = FVector::ZeroVector;
	}
}

void UAutonomousMovementComponent::SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget)
{
	if(NewTarget.IsValid() && ChaseTarget != NewTarget)
	{
		ChaseTarget = NewTarget;
	}
}

bool UAutonomousMovementComponent::IsSeeker() const
{
	return bIsSeeker;
}
