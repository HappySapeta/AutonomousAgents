
#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Behaviours/Base/BaseFlockingBehaviour.h"
#include "Behaviours/Base/FlockingInterface.h"
#include "CohesionBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class AUTONOMOUSAGENTS_API UCohesionBehaviour : public UBaseFlockingBehaviour, public IFlockingInterface
{
	GENERATED_BODY()

public:
	virtual FVector CalculateSteerForce(const FAgentData* AgentData, const TArray<const FAgentData*>& OtherActors,const float MaxSpeed) const override;
};
