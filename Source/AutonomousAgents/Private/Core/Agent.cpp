#include "Core/Agent.h"

#include "Kismet/KismetMathLibrary.h"

#define RESERVATION_SIZE 100

UAgent::UAgent()
{
	NearbyAgentIndices.Reserve(RESERVATION_SIZE);
}

FVector UAgent::GetForwardVector() const
{
	return ForwardVector;
}

void UAgent::UpdateState(const float DeltaSeconds)
{
	const FVector& NewVelocity = Velocity + MovementForce * DeltaSeconds;
	const FVector& NewLocation = Location + NewVelocity * DeltaSeconds;

	Velocity = NewVelocity;
	Location = NewLocation;
	MovementForce = FVector::ZeroVector;

	AlignForwardWithVelocity(DeltaSeconds);
}

void UAgent::AlignForwardWithVelocity(const float DeltaSeconds)
{
	const FVector& TargetDirection = Velocity.GetSafeNormal();
	ForwardVector = UKismetMathLibrary::VLerp(ForwardVector, TargetDirection, 0.5f).GetSafeNormal();
}
