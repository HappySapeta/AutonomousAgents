
#pragma once

#include <CoreMinimal.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/SeekingInterface.h"
#include "DirectSeekBehaviour.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API UDirectSeekBehaviour : public UBaseAutonomousBehaviour, public ISeekingInterface
{
	GENERATED_BODY()

public:
	
	virtual FVector CalculateSeekForce(const UAgent* AffectedAgentData, const AActor* ChaseTarget, const float MaxSpeed) const override;
	
};
