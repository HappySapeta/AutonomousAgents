
#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>
#include "AgentPawn.generated.h"

// Forward declarations
class USphereComponent;
class UAutonomousMovementComponent;


/**
 * AgentPawn represents an autonomous agent.
 * It has the ability to sense other agents and perform steering maneuvers
 * to create interesting movement patterns similar to flocks of sheep or birds.
 *
 * Its movement is controlled by the AutonomousMovementComponent.
 * Edit that component to change the agent's movement pattern.
 */
UCLASS(Blueprintable)
class AAgentPawn : public APawn
{
	GENERATED_BODY()

public:

	// Sets up components.
	AAgentPawn();
	
	// Rotates the actor to align the actor with its velocity. 
	//void AlignActorToVelocity(const FVector& Velocity, float DeltaTime);

protected:

	// Defines the speed at which the actor aligns with its velocity.
	UPROPERTY(EditDefaultsOnly, Category = "Rotation", meta = (ClampMin = "0.0", ClampMax = "100.0", RangeMin = "0.0", RangeMax = "100.0"))
	float VelocityAlignmentSpeed = 1.0f;
};

