// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(const FWeakActorPtr& Affector, const FActorArray& NearbyAgents, const float MaxSpeed) const
{
	if(!bIsEnabled || !Affector.IsValid()) return FVector::ZeroVector;
	
	FVector SteeringInput = FVector::ZeroVector;

	FActorArray SeparationAgents;
	GetAgentsInView(Affector, NearbyAgents, SeparationAgents);
	
	if(SeparationAgents.Num() > 0)
	{
		FVector AvoidanceVector = FVector::ZeroVector;

		for(const TWeakObjectPtr<AActor>& OtherAgent : SeparationAgents)
		{
			const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
			FVector OtherAgentVector = Affector->GetActorLocation() - OtherAgentLocation;
			const float OtherAgentDistance = OtherAgentVector.Length();
			
			OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
			AvoidanceVector += OtherAgentVector;
			
			if(bShouldDebug)
			{
				DrawDebugLine(GetWorld(), Affector->GetActorLocation(), OtherAgentLocation, FColor::Red, false, 0.02f, 0, 5.0f);
			}
		}

		AvoidanceVector /= SeparationAgents.Num();
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - Affector->GetVelocity();
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
