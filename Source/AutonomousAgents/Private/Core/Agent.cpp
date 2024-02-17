#include "Core/Agent.h"

#include "Kismet/KismetMathLibrary.h"

constexpr uint32 GNearbyAgentsSize = 100;

UAgent::UAgent()
{
	memset(NearbyAgentIndices.GetData(), 0, sizeof(NearbyAgentIndices));
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

void UAgent::SetVelocityAlignmentSpeed(const float Speed)
{
	VelocityAlignmentSpeed = FMath::Clamp(Speed, 0.001f, 1.0f);
}

void UAgent::AlignForwardWithVelocity()
{
	const FVector& TargetDirection = Velocity.GetSafeNormal();
	ForwardVector = UKismetMathLibrary::VLerp(ForwardVector, TargetDirection, VelocityAlignmentSpeed).GetSafeNormal();
}
