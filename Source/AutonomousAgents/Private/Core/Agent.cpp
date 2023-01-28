#include "Core/Agent.h"

#define RESERVATION_SIZE 100

UAgent::UAgent()
{
	NearbyAgentIndices.Reserve(RESERVATION_SIZE);
}

FVector UAgent::GetForwardVector() const
{
	return Velocity.GetSafeNormal();
}

void UAgent::UpdateState(float DeltaTime)
{
	const FVector& NewVelocity = Velocity + MovementForce * DeltaTime;
	const FVector& NewLocation = Location + NewVelocity * DeltaTime;

	Velocity = NewVelocity;
	Location = NewLocation;
	MovementForce = FVector::ZeroVector;
}
