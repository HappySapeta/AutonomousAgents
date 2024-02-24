#pragma once
#include "SpatialAcceleration/RpImplicitGrid.h"
#include "Agent.generated.h"

/**
 * UAgent represents an agent in the simulation space.
 * It contains all information that is essential
 * to determine its current physical state such as velocity,and location.
 * All behavioural algorithms work upon instances of this class.
 */
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
	 * @param DeltaSeconds Amount of time elapsed since the last update.
	 */
	void UpdateState(const float DeltaSeconds);

	void SetVelocityAlignmentSpeed(const float Speed);

public:
	
	FRpSearchResults NearbyAgentIndices;
	uint32 NumNearbyAgents = 0; 
	FVector Location = FVector::ZeroVector;
	FVector Velocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
	FVector ForwardVector = FVector::ZeroVector;
	
private:
	
	/**
	 * @brief Interpolates the forward vector to align with the velocity direction.
	 */
	void AlignForwardWithVelocity();

private:

	// The speed at which the agent aligns itself along its velocity.
	float VelocityAlignmentSpeed = 0.1f;
	
};
