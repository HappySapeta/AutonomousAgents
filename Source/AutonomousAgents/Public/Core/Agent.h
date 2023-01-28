#pragma once
#include "Agent.generated.h"

UCLASS(NotBlueprintable)
class AUTONOMOUSAGENTS_API UAgent : public UObject
{
	GENERATED_BODY()
	
public:

	// Reserves memory for NearbyIndicesArray.
	UAgent();

	// Returns the Agent's forward direction.
	FVector GetForwardVector() const;

	
	/**
	 * @brief Perform physics update (location and velocity) on a certain agent, using a variable delta-time.
	 * @param DeltaTime Amount of time elapsed since the last update.
	 */
	void UpdateState(float DeltaTime);
	
	TArray<uint32> NearbyAgentIndices;
	FVector Location = FVector::ZeroVector;
	FVector Velocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
	
};
