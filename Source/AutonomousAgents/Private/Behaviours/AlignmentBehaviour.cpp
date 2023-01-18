
#include "Behaviours/AlignmentBehaviour.h"

FVector UAlignmentBehaviour::CalculateSteerForce(const FAgentData& AffectedAgentData, const FActorArray* OtherActors, const float MaxSpeed) const
{
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector AverageFlockVelocity = FVector::ZeroVector;
	uint32 NumAlignmentAgents = 0;

	for (const uint32 Index : AffectedAgentData.NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = OtherActors->operator[](Index);
		if (!OtherAgent.IsValid() || !CanOtherAgentAffect(AffectedAgentData, OtherAgent))
		{
			continue;
		}

		++NumAlignmentAgents;
		AverageFlockVelocity += OtherAgent->GetVelocity();

		if (bDebug)
		{
			DrawDebugLine(AffectedAgentData.AffectedActor->GetWorld(), AffectedAgentData.Location, OtherAgent->GetActorLocation(), DebugColor);
		}
	}

	if (NumAlignmentAgents > 0)
	{
		AverageFlockVelocity /= NumAlignmentAgents;
		AverageFlockVelocity = AverageFlockVelocity.GetSafeNormal() * MaxSpeed;

		const FVector& AlignmentManeuver = AverageFlockVelocity - AffectedAgentData.Velocity;
		SteeringInput = AlignmentManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
