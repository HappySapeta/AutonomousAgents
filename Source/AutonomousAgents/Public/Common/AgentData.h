#pragma once
#include "AgentData.generated.h"

UCLASS()
class AUTONOMOUSAGENTS_API UAgentData : public UObject
{
	GENERATED_BODY()
	
public:

	UAgentData() {}
	
	FVector GetForwardVector() const
	{
		return Velocity.GetSafeNormal();
	}

	void UpdateState(float DeltaTime)
	{
		const FVector& NewVelocity = Velocity + MovementForce * DeltaTime;
		const FVector& NewLocation = Location + NewVelocity * DeltaTime;

		Velocity = NewVelocity;
		Location = NewLocation;
		MovementForce = FVector::ZeroVector;
	}
	
public:
	
	FVector Location = FVector::ZeroVector;
	FVector Velocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
	TArray<uint32> NearbyAgentIndices;
	
};
