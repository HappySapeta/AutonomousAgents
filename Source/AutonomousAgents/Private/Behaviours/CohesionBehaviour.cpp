
#include "Behaviours/CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(
	const FWeakActorPtr& AffectedActor,
	const FActorArray* AllActors, const TArray<uint32>& NearbyAgentIndices,
	const float MaxSpeed) const
{
	if (!bIsEnabled || !AffectedActor.IsValid()) return FVector::ZeroVector;

	FVector SteeringInput = FVector::ZeroVector;
	FVector HerdLocation = FVector::ZeroVector;
	uint32 NumCohesiveAgents = 0;

	for (const uint32 Index : NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = AllActors->operator[](Index);
		if (!OtherAgent.IsValid() || !CanAgentAffect(AffectedActor, OtherAgent))
		{
			continue;
		}

		++NumCohesiveAgents;
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		HerdLocation += OtherAgentLocation;

		if (bShouldDebug)
		{
			DrawDebugLine(GetWorld(), AffectedActor->GetActorLocation(), OtherAgentLocation, FColor::Blue);
		}
	}

	if (NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - AffectedActor->GetActorLocation()).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - AffectedActor->GetVelocity();
		SteeringInput = CohesionManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
