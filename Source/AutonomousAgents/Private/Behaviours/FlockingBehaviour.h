// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "DataTypes/CommonTypes.h"
#include "FlockingBehaviour.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UFlockingBehaviour : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AUTONOMOUSAGENTS_API IFlockingBehaviour
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FVector CalculateSteerForce(const FWeakActorPtr& AffectedActor, const FActorArray& NearbyAgents, const float MaxSpeed) const { return FVector::ZeroVector; }
};
