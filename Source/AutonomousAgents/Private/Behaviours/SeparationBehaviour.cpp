// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(
	const FWeakActorPtr& AffectedActor,
	const FActorArray* AllActors, const TArray<uint32>& NearbyAgentIndices,
	const float MaxSpeed) const
{
	if(!bIsEnabled || !AffectedActor.IsValid()) return FVector::ZeroVector;
	
	FVector SteeringInput = FVector::ZeroVector;
	FVector AvoidanceVector = FVector::ZeroVector;
	uint32 NumAvoidableAgents = 0;
	
	for(const uint32 Index : NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = AllActors->operator[](Index);
		if(!OtherAgent.IsValid() || !CanAgentAffect(AffectedActor, OtherAgent))
		{
			continue;
		}

		++NumAvoidableAgents;
		
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
		FVector OtherAgentVector = AffectedActor->GetActorLocation() - OtherAgentLocation;
		const float OtherAgentDistance = OtherAgentVector.Length();
			
		OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
		AvoidanceVector += OtherAgentVector;
			
		if(bShouldDebug)
		{
			DrawDebugLine(GetWorld(), AffectedActor->GetActorLocation(), OtherAgentLocation, FColor::Red, false);
		}
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - AffectedActor->GetVelocity();
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}
	
	return SteeringInput;
}
