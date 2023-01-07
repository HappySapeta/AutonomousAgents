// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviours/BaseAutonomousBehaviour.h"
#include "Behaviours/SeekingBehaviour.h"
#include "UObject/Object.h"
#include "DirectSeekBehaviour.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API UDirectSeekBehaviour : public UBaseAutonomousBehaviour, public ISeekingBehaviour
{
	GENERATED_BODY()

public:
	
	virtual FVector CalculateSeekForce(const FWeakActorPtr& AffectedActor, const FWeakActorPtr& ChaseTarget, const float MaxSpeed) const override;

protected:

	
};
