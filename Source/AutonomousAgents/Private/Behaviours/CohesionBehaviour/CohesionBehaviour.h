// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Behaviours/BaseAutonomousBehaviour.h"
#include "Behaviours/FlockingBehaviour.h"
#include "CohesionBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API UCohesionBehaviour : public UBaseAutonomousBehaviour, public IFlockingBehaviour
{
	GENERATED_BODY()

public:
	
	virtual FVector CalculateSteerForce(const FWeakActorPtr& Affector, const FActorArray& NearbyAgents, const float MaxSpeed) const override;
	
public:
	
};
