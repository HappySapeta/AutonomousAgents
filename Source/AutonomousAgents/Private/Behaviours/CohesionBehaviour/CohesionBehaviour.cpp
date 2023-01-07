// Fill out your copyright notice in the Description page of Project Settings.

#include "CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(const FWeakActorPtr& Affector, const FActorArray& NearbyAgents, const float MaxSpeed) const
{
	if(!bIsEnabled || !Affector.IsValid()) return FVector::ZeroVector;
	
	FVector SteeringInput = FVector::ZeroVector;

	FActorArray CohesionAgents;
	GetAgentsInView(Affector, NearbyAgents, CohesionAgents);
	
	if(CohesionAgents.Num() > 0)
	{
		FVector HerdLocation = FVector::ZeroVector;
		for(const FWeakActorPtr& OtherAgent : CohesionAgents)
		{
			const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
			HerdLocation += OtherAgentLocation;
			
			if(bShouldDebug)
			{
				DrawDebugLine(GetWorld(), Affector->GetActorLocation(), OtherAgentLocation, FColor::Blue, false, 0.02f, 0, 5.0f);
			}
		}

		HerdLocation /= CohesionAgents.Num();
		const FVector& DesiredVelocity = (HerdLocation - Affector->GetActorLocation()).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - Affector->GetVelocity();
		SteeringInput = CohesionManeuver * Influence;
	}

	return SteeringInput;
}
