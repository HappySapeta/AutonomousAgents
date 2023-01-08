// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/AlignmentBehaviour.h"

FVector UAlignmentBehaviour::CalculateSteerForce(const FWeakActorPtr& Affector, const FActorArray& NearbyAgents, const float MaxSpeed) const
{
	if(!bIsEnabled || !Affector.IsValid()) return FVector::ZeroVector;

	FVector SteeringInput = FVector::ZeroVector;

	FActorArray AlignmentAgents;
	GetAgentsInView(Affector, NearbyAgents, AlignmentAgents);
	
	if(AlignmentAgents.Num() > 0)
	{
		FVector AverageFlockVelocity = FVector::ZeroVector;
		for(const TWeakObjectPtr<AActor>& OtherAgent : AlignmentAgents)
		{
			AverageFlockVelocity += OtherAgent->GetVelocity();
			
			if(bShouldDebug)
			{
				DrawDebugLine(GetWorld(), Affector->GetActorLocation(), OtherAgent->GetActorLocation(), FColor::Yellow, false, 0.02f, 0, 5.0f);
			}
		}

		AverageFlockVelocity /= AlignmentAgents.Num();
		AverageFlockVelocity = AverageFlockVelocity.GetSafeNormal() * MaxSpeed;

		const FVector& AlignmentManeuver = AverageFlockVelocity - Affector->GetVelocity();
		SteeringInput = AlignmentManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
