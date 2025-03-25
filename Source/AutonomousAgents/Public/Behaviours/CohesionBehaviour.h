#pragma once

#include <CoreMinimal.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "CohesionBehaviour.generated.h"

/**
 * Makes the agents clump together.
 */
UCLASS()
class AUTONOMOUSAGENTS_API UCohesionBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const UAgent* Agent, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const override;
};
