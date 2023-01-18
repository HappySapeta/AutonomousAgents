#pragma once

typedef TWeakObjectPtr<AActor> FWeakActorPtr;
typedef TArray<FWeakActorPtr> FActorArray;

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
	
	FVector Location;
	FVector Velocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
	
	TArray<uint32> NearbyAgentIndices;
	TWeakObjectPtr<AActor> AffectedActor;
};
