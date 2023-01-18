
#include "Behaviours/SeparationBehaviour.h"

FVector USeparationBehaviour::CalculateSteerForce(const FAgentData& AffectedAgentData, const FActorArray* OtherActors, const float MaxSpeed) const
{
	if(!bIsEnabled)
	{
		return FVector::ZeroVector;
	}
	
	FVector SteeringInput = FVector::ZeroVector;
	FVector AvoidanceVector = FVector::ZeroVector;
	uint32 NumAvoidableAgents = 0;
	
	for(const uint32 Index : AffectedAgentData.NearbyAgentIndices)
	{
		const FWeakActorPtr& OtherAgent = OtherActors->operator[](Index);
		if(!OtherAgent.IsValid() || !CanOtherAgentAffect(AffectedAgentData, OtherAgent))
		{
			continue;
		}

		++NumAvoidableAgents;
		
		const FVector& OtherAgentLocation = OtherAgent->GetActorLocation();
		
		FVector OtherAgentVector = AffectedAgentData.Location - OtherAgentLocation;
		const float OtherAgentDistance = OtherAgentVector.Length();
			
		OtherAgentVector = OtherAgentVector.GetSafeNormal() / OtherAgentDistance;
		AvoidanceVector += OtherAgentVector;
			
		if(bDebug)
		{
			DrawDebugLine(AffectedAgentData.AffectedActor->GetWorld(), AffectedAgentData.Location, OtherAgent->GetActorLocation(), DebugColor);
		}
	}

	if(NumAvoidableAgents > 0)
	{
		AvoidanceVector /= NumAvoidableAgents;
		AvoidanceVector = AvoidanceVector.GetSafeNormal() * MaxSpeed;

		const FVector& SeparationManeuver = AvoidanceVector - AffectedAgentData.Velocity;
		SteeringInput = SeparationManeuver * Influence * InfluenceScale;
	}
	
	return SteeringInput;
}
