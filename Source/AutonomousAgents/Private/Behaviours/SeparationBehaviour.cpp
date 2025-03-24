
#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(const UAgent* Agent, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SeparationBehaviour::CalculateSteerForce)
	if(!bIsEnabled)
	{
		return FVector::ZeroVector;
	}
	
	FVector SteeringInput = FVector::ZeroVector;
	FVector AvoidanceVector = FVector::ZeroVector;
	uint32 NumAvoidableAgents = 0;
	
	for(uint32 NeighborIndex : Agent->NearbyAgentIndices)
	{
		const UAgent* Neighbor = OtherAgents[NeighborIndex];
		if(!CanOtherAgentAffect(Agent, Neighbor))
		{
			continue;
		}
		
		++NumAvoidableAgents;
		
		const FVector& NeighborLocation = Neighbor->Location;
		
		FVector NeighborVector = Agent->Location - NeighborLocation;
		NeighborVector = NeighborVector.GetSafeNormal() / NeighborVector.Length();
		AvoidanceVector += NeighborVector;
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - Agent->Velocity;
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}
	
	return SteeringInput;
}
