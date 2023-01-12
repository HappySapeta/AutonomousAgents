// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(const FWeakActorPtr& SelfAgent, const FActorArray& NearbyAgents, const float MaxSpeed) const
{
	if(!bIsEnabled || !SelfAgent.IsValid()) return FVector::ZeroVector;
	
	FVector SteeringInput = FVector::ZeroVector;
	FVector AvoidanceVector = FVector::ZeroVector;
	uint32 NumAvoidableAgents = 0;
	
	for(const TWeakObjectPtr<AActor>& OtherAgent : NearbyAgents)
	{
		if(!OtherAgent.IsValid() || !CanAgentAffect(SelfAgent, OtherAgent))
		{
			continue;
		}

		++NumAvoidableAgents;
		
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
		FVector OtherAgentVector = SelfAgent->GetActorLocation() - OtherAgentLocation;
		const float OtherAgentDistance = OtherAgentVector.Length();
			
		OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
		AvoidanceVector += OtherAgentVector;
			
		if(bShouldDebug)
		{
			DrawDebugLine(GetWorld(), SelfAgent->GetActorLocation(), OtherAgentLocation, FColor::Red, false, 0.02f, 0, 5.0f);
		}
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - SelfAgent->GetVelocity();
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}
	
	return SteeringInput;
}
