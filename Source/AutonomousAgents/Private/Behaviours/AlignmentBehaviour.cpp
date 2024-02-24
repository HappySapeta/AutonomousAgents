
#include "Behaviours/AlignmentBehaviour.h"

#include "Core/Agent.h"

FVector UAlignmentBehaviour::CalculateSteerForce(const UAgent* AffectedAgentData, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UAlignmentBehaviour::CalculateSteerForce)
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector AverageFlockVelocity = FVector::ZeroVector;
	uint32 NumAlignmentAgents = 0;

	const FRpSearchResults& NearbyAgents = AffectedAgentData->NearbyAgentIndices;
	uint8 Count = NearbyAgents.Num();
	for(auto Itr = NearbyAgents.Array.begin(); Count > 0; --Count, ++Itr)
	{
		const UAgent* OtherAgent = OtherAgents[*Itr];
		if (!CanOtherAgentAffect(AffectedAgentData, OtherAgent))
		{
			continue;
		}

		++NumAlignmentAgents;
		AverageFlockVelocity += OtherAgent->Velocity;
	}

	if (NumAlignmentAgents > 0)
	{
		AverageFlockVelocity /= NumAlignmentAgents;
		AverageFlockVelocity = AverageFlockVelocity.GetSafeNormal() * MaxSpeed;

		const FVector& AlignmentManeuver = AverageFlockVelocity - AffectedAgentData->Velocity;
		SteeringInput = AlignmentManeuver * Influence * InfluenceScale;
	}

	return SteeringInput;
}
