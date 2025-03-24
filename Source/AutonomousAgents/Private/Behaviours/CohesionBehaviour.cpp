
#include "Behaviours/CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(const UAgent* Agent, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UCohesionBehaviour::CalculateSteerForce)
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector HerdLocation = FVector::ZeroVector;
	uint32 NumCohesiveAgents = 0;

	for(uint32 NeighborIndex : Agent->NearbyAgentIndices)
	{
		const UAgent* Neighbor = OtherAgents[NeighborIndex];
		if(!CanOtherAgentAffect(Agent, Neighbor))
		{
			continue;
		}

		++NumCohesiveAgents;
		HerdLocation += Neighbor->Location;
	}

	if (NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - Agent->Location).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - Agent->Velocity;
		SteeringInput = CohesionManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
