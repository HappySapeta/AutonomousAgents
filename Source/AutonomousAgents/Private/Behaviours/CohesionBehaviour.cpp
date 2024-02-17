
#include "Behaviours/CohesionBehaviour.h"

FVector UCohesionBehaviour::CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherActors, const float MaxSpeed) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UCohesionBehaviour::CalculateSteerForce)
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector HerdLocation = FVector::ZeroVector;
	uint32 NumCohesiveAgents = 0;

	const uint32 NumNearbyAgents = AgentData->NumNearbyAgents;
	const FRpGridSearchResult& NearbyAgents = AgentData->NearbyAgentIndices;
	for (uint32 Index = 0; Index < NumNearbyAgents; ++Index)
	{
		const UAgent* OtherAgent = OtherActors[NearbyAgents[Index]];
		if (!CanOtherAgentAffect(AgentData, OtherAgent))
		{
			continue;
		}

		++NumCohesiveAgents;
		const FVector& OtherAgentLocation = OtherAgent->Location;
		HerdLocation += OtherAgentLocation;
	}

	if (NumCohesiveAgents > 0)
	{
		HerdLocation /= NumCohesiveAgents;
		const FVector& DesiredVelocity = (HerdLocation - AgentData->Location).GetSafeNormal() * MaxSpeed;
		const FVector& CohesionManeuver = DesiredVelocity - AgentData->Velocity;
		SteeringInput = CohesionManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
