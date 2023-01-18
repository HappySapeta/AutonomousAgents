
#include "Behaviours/DirectSeekBehaviour.h"

FVector UDirectSeekBehaviour::CalculateSeekForce(const FAgentData& AgentData, const AActor* ChaseTarget, const float MaxSpeed) const
{
	if(!bIsEnabled || !ChaseTarget)
	{
		return FVector::ZeroVector;
	}
	
	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - AgentData.Location;
	
	DesiredVelocity.Normalize();
	DesiredVelocity *= MaxSpeed;

	const FVector& ChaseManeuver = DesiredVelocity - AgentData.Velocity;

	if(bDebug)
	{
		DrawDebugLine(AgentData.AffectedActor->GetWorld(), AgentData.Location, ChaseTarget->GetActorLocation(), DebugColor);
	}
	
	return ChaseManeuver * Influence * InfluenceScale;
}
