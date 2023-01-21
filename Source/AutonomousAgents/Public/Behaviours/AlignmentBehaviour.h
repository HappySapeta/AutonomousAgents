
#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "AlignmentBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API UAlignmentBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const FAgentData* AgentData, const TArray<const FAgentData*>& OtherAgents, const float MaxSpeed) const override;
};
