#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "Core/Agent.h"
#include "FlockingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UFlockingInterface : public UInterface
{	
	GENERATED_BODY()
};

/**
 * Interface for all flocking behaviours.
 */
class AUTONOMOUSAGENTS_API IFlockingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector CalculateSteerForce(const UAgent* AgentData, const TArray<UAgent*>& OtherAgents, const float MaxSpeed) const
	{
		return FVector::ZeroVector;
	}
};
