
#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "SeparationBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API USeparationBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const FAgentData* AgentData, const TArray<const FAgentData*>& OtherActors, const float MaxSpeed) const override;
};
