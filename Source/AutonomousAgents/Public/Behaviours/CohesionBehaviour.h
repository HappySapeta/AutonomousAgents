// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "CohesionBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API UCohesionBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const FWeakActorPtr& AffectedActor, const FActorArray* AllActors,
	                                    const TArray<uint32>& NearbyAgentIndices, const float MaxSpeed) const override;
};
