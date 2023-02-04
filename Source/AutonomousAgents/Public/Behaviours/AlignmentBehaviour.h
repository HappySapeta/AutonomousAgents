#pragma once

#include <CoreMinimal.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "AlignmentBehaviour.generated.h"

/**
 * Makes an agent move along in the direction of other agents.
 */
UCLASS()
class AUTONOMOUSAGENTS_API UAlignmentBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const override;
};
