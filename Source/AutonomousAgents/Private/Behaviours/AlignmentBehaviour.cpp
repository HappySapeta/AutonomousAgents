
#include "Behaviours/AlignmentBehaviour.h"

#include "Common/AgentData.h"

FVector UAlignmentBehaviour::CalculateSteerForce(const FAgentData* AffectedAgentData, const TArray<const FAgentData*>& OtherAgents, const float MaxSpeed) const
{
	if (!bIsEnabled)
	{
		return FVector::ZeroVector;
	}

	FVector SteeringInput = FVector::ZeroVector;
	FVector AverageFlockVelocity = FVector::ZeroVector;
	uint32 NumAlignmentAgents = 0;

	for (const uint32 Index : AffectedAgentData->NearbyAgentIndices)
	{
		const FAgentData* OtherAgent = OtherAgents[Index];
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
