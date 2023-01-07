// Fill out your copyright notice in the Description page of Project Settings.

#include "Behaviours/DirectSeekBehaviour.h"

FVector UDirectSeekBehaviour::CalculateSeekForce(const FWeakActorPtr& AffectedActor, const FWeakActorPtr& ChaseTarget, const float MaxSpeed) const
{
	if(!bIsEnabled) return FVector::ZeroVector;
	if(!ChaseTarget.IsValid() || !AffectedActor.IsValid()) return FVector::ZeroVector;

	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - AffectedActor->GetActorLocation();
	
	DesiredVelocity.Normalize();
	DesiredVelocity *= MaxSpeed;

	const FVector& ChaseManeuver = DesiredVelocity - AffectedActor->GetVelocity();

	if(bShouldDebug)
	{
		DrawDebugLine(GetWorld(), AffectedActor->GetActorLocation(), ChaseTarget->GetActorLocation(), FColor::Emerald, false, 0.01f, 0, 5.0f);
	}
	
	return ChaseManeuver * Influence;
}
