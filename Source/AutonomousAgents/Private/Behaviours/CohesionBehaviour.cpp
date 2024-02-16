
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

	for (int32 Index : AgentData->NearbyAgentIndices)
	{
		if(Index == -1)
		{
			break;
		}
		
		const UAgent* OtherAgent = OtherActors[Index];
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
