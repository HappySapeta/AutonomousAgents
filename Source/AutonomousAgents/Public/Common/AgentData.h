#pragma once

struct FAgentData
{
	FAgentData(const TWeakObjectPtr<AActor>& Actor)
	{
		AffectedActor = Actor;
		Velocity = AffectedActor->GetVelocity();
		Location = AffectedActor->GetActorLocation();
	}

	bool operator==(const FAgentData& Other) const
	{
		return Other.AffectedActor == this->AffectedActor;
	}

	FVector GetForwardVector() const
	{
		return Velocity.GetSafeNormal();
	}
	
	FVector Location;
	FVector Velocity;
	FVector MovementForce = FVector::ZeroVector;
	
	TArray<uint32> NearbyAgentIndices;

private:
	TWeakObjectPtr<AActor> AffectedActor;
};