
#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "Core/Agent.h"
#include "SeekingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class USeekingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AUTONOMOUSAGENTS_API ISeekingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FVector CalculateSeekForce(const UAgent* AgentData, const AActor* ChaseTarget, const float MaxSpeed) const { return FVector::ZeroVector; }
	
};
