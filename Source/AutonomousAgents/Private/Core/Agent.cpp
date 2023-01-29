#include "Core/Agent.h"

#include "Kismet/KismetMathLibrary.h"

constexpr uint32 GNearbyAgentsSize = 100;
constexpr float GVelocityAlignmentSpeed = 0.5f; 

UAgent::UAgent()
{
	NearbyAgentIndices.Reserve(GNearbyAgentsSize);
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

	AlignForwardWithVelocity();
}

void UAgent::AlignForwardWithVelocity()
{
	const FVector& TargetDirection = Velocity.GetSafeNormal();
	ForwardVector = UKismetMathLibrary::VLerp(ForwardVector, TargetDirection, GVelocityAlignmentSpeed).GetSafeNormal();
}
