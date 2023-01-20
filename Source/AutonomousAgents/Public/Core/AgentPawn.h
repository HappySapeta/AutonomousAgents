
#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>
#include "Common/AgentData.h"
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
	
	virtual FVector GetVelocity() const override;

	void SetData(const FAgentData* Data);

protected:

	// Finds chase targets and initializes some variables.
	virtual void BeginPlay() override;

	// 1. Calculate current velocity.
	// 2. Align actor with velocity.
	virtual void Tick(float DeltaSeconds) override;

private:

	// Rotates the actor to align the actor with its velocity. 
	void AlignActorToVelocity(float DeltaSeconds);

	// Calculate current velocity using time elapsed and previous location.
	void CalculateCurrentVelocity(float DeltaSeconds);

protected:

	// Defines the speed at which the actor aligns with its velocity.
	UPROPERTY(EditDefaultsOnly, Category = "Rotation", meta = (ClampMin = "0.0", ClampMax = "100.0", RangeMin = "0.0", RangeMax = "100.0"))
	float VelocityAlignmentSpeed = 1.0f;

private:

	const FAgentData* AgentData;
	FVector CurrentVelocity = FVector::ZeroVector;
	FVector PreviousLocation;
};

