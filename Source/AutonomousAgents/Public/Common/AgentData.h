#pragma once
#include "Core/AgentPawn.h"
#include "AgentData.generated.h"

UCLASS()
class AUTONOMOUSAGENTS_API UAgentData : public UObject
{
	GENERATED_BODY()
	
public:

	UAgentData() {}
	
	bool operator==(const UAgentData& Other) const
	{
		return Other.AffectedAgentActor == this->AffectedAgentActor;
	}

	void SetAffectedActor(AAgentPawn* AgentActor)
	{
		checkf(AgentActor, TEXT("AgentActor cannot be null."))
		AffectedAgentActor = AgentActor;
		Velocity = AffectedAgentActor->GetVelocity();
		Location = AffectedAgentActor->GetActorLocation();
	}
	
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

		AffectedAgentActor->SetActorLocation(Location);
		AffectedAgentActor->AlignActorToVelocity(Velocity, DeltaTime);
	}
	
public:
	
	FVector Location = FVector::ZeroVector;
	FVector Velocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
	TArray<uint32> NearbyAgentIndices;
	
private:

	UPROPERTY(Transient)
	AAgentPawn* AffectedAgentActor;
	
};
