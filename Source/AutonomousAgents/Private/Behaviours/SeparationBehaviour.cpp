﻿
#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherActors, const float MaxSpeed) const
{
	if(!bIsEnabled)
	{
		return FVector::ZeroVector;
	}
	
	FVector SteeringInput = FVector::ZeroVector;
	FVector AvoidanceVector = FVector::ZeroVector;
	uint32 NumAvoidableAgents = 0;
	
	for(const uint32 Index : AgentData->NearbyAgentIndices)
	{
		const UAgent* OtherAgent = OtherActors[Index];
		if(!CanOtherAgentAffect(AgentData, OtherAgent))
		{
			continue;
		}

		++NumAvoidableAgents;
		
		const FVector& OtherAgentLocation = OtherAgent->Location;
		
		FVector OtherAgentVector = AgentData->Location - OtherAgentLocation;
		const float OtherAgentDistance = OtherAgentVector.Length();
			
		OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
		AvoidanceVector += OtherAgentVector;
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - AgentData->Velocity;
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}
	
	return SteeringInput;
}
