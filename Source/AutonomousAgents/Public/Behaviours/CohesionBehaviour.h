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
	virtual FVector CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherActors, const float MaxSpeed) const override;
};
