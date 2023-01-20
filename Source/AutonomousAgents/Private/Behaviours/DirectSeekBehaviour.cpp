
#include "Behaviours/DirectSeekBehaviour.h"
#include "Common/AgentData.h"

FVector UDirectSeekBehaviour::CalculateSeekForce(const FAgentData* AffectedAgentData, const AActor* ChaseTarget, const float MaxSpeed) const
{
	if(!bIsEnabled || !ChaseTarget)
	{
		return FVector::ZeroVector;
	}
	
	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - AffectedAgentData->Location;
	
	DesiredVelocity.Normalize();
	DesiredVelocity *= MaxSpeed;

	const FVector& ChaseManeuver = DesiredVelocity - AffectedAgentData->Velocity;

	if(bDebug)
	{
		DrawDebugLine(ChaseTarget->GetWorld(), AffectedAgentData->Location, ChaseTarget->GetActorLocation(), DebugColor);
	}
	
	return ChaseManeuver * Influence * InfluenceScale;
}
