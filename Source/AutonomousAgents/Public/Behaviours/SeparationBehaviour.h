#pragma once

#include <CoreMinimal.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "SeparationBehaviour.generated.h"

/**
 * Moves the agent to avoid other agents.
 */
UCLASS()
class AUTONOMOUSAGENTS_API USeparationBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherActors, const float MaxSpeed) const override;
};
