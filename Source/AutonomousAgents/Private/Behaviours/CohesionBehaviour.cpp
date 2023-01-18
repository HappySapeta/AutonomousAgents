
#include "Behaviours/CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(const FAgentData& AffectedAgentData, const FActorArray* OtherActors,
	const float MaxSpeed) const
{
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector HerdLocation = FVector::ZeroVector;
	uint32 NumCohesiveAgents = 0;

	for (const uint32 Index : AffectedAgentData.NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = OtherActors->operator[](Index);
		if (!OtherAgent.IsValid() || !CanOtherAgentAffect(AffectedAgentData, OtherAgent))
		{
			continue;
		}

		++NumCohesiveAgents;
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		HerdLocation += OtherAgentLocation;

		if (bDebug)
		{
			DrawDebugLine(AffectedAgentData.AffectedActor->GetWorld(), AffectedAgentData.Location, OtherAgent->GetActorLocation(), DebugColor);
		}
	}

	if (NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - AffectedAgentData.Location).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - AffectedAgentData.Velocity;
		SteeringInput = CohesionManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
