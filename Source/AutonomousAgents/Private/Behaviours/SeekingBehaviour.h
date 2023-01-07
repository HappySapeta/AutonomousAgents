// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "DataTypes/CommonTypes.h"
#include "SeekingBehaviour.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USeekingBehaviour : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AUTONOMOUSAGENTS_API ISeekingBehaviour
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FVector CalculateSeekForce(const FWeakActorPtr& AffectedActor, const FWeakActorPtr& ChaseTarget, const float MaxSpeed) const { return FVector::ZeroVector; }
	
};
