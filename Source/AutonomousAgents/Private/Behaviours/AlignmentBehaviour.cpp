// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/AlignmentBehaviour.h"

FVector UAlignmentBehaviour::CalculateSteerForce(
	const FWeakActorPtr& SelfAgent, const FActorArray* AllActors,
	const TArray<uint32>& NearbyAgentIndices,
	const float MaxSpeed) const
{
	if (!bIsEnabled || !SelfAgent.IsValid()) return FVector::ZeroVector;

	FVector SteeringInput = FVector::ZeroVector;
	FVector AverageFlockVelocity = FVector::ZeroVector;
	uint32 NumAlignmentAgents = 0;

	for (const uint32 Index : NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = AllActors->operator[](Index);
		if (!OtherAgent.IsValid() || !CanAgentAffect(SelfAgent, OtherAgent))
		{
			continue;
		}

		++NumAlignmentAgents;
		AverageFlockVelocity += OtherAgent->GetVelocity();

		if (bShouldDebug)
		{
			DrawDebugLine(GetWorld(), SelfAgent->GetActorLocation(), OtherAgent->GetActorLocation(), FColor::Yellow, false);
		}
	}

	if (NumAlignmentAgents > 0)
	{
		AverageFlockVelocity /= NumAlignmentAgents;
		AverageFlockVelocity = AverageFlockVelocity.GetSafeNormal() * MaxSpeed;

		const FVector& AlignmentManeuver = AverageFlockVelocity - SelfAgent->GetVelocity();
		SteeringInput = AlignmentManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
