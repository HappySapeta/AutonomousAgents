// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(const FWeakActorPtr& SelfAgent, const FActorArray& NearbyAgents,
                                                const float MaxSpeed) const
{
	if (!bIsEnabled || !SelfAgent.IsValid()) return FVector::ZeroVector;

	FVector SteeringInput = FVector::ZeroVector;
	FVector HerdLocation = FVector::ZeroVector;
	uint32 NumCohesiveAgents = 0;

	for (const FWeakActorPtr& OtherAgent : NearbyAgents)
	{
		if(!OtherAgent.IsValid() || !CanAgentAffect(SelfAgent, OtherAgent))
		{
			continue;
		}

		++NumCohesiveAgents;
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		HerdLocation += OtherAgentLocation;

		if (bShouldDebug)
		{
			DrawDebugLine(GetWorld(), SelfAgent->GetActorLocation(), OtherAgentLocation, FColor::Blue);
		}
	}

	if (NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - SelfAgent->GetActorLocation()).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - SelfAgent->GetVelocity();
		SteeringInput = CohesionManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
