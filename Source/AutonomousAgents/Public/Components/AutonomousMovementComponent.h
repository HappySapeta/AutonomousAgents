// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "Common/CommonTypes.h"
#include "Common/FSearchParameters.h"
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class USpatialGridSubsystem;
class UBaseAutonomousBehaviour;

/**
 * AutonomousMovementComponent applies a series of
 * Craig Reynold's autonomous steering behaviour on an actor.
 *
 * It uses overlap triggers to sense and retrieve information about other agents
 * do calculate the actor's movement velocity.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AUTONOMOUSAGENTS_API UAutonomousMovementComponent : public UActorComponent
{
	
	GENERATED_BODY()

public:

	// 1. Initialize class defaults
	// 2. Bind events
	UAutonomousMovementComponent();

	// 1. Apply flocking behaviour.
	// 2. Update physics.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetChaseTarget(const FWeakActorPtr& NewTarget);

protected:

	// 1. Initialize variables.
	virtual void BeginPlay() override;
	
	// Is the agent not surrounded by other agents.
	bool CanAgentLead() const;

private:

	// Calculate velocity and new location from the net Movement Force.
	virtual void PhysicsUpdate(float DeltaTime);

	void InvokeBehaviours();
	
	void ResetBehaviours();
	
	void FetchGridSubsystem();

	void SenseNearbyAgents();
	
protected:

	// Maximum speed of the agent.
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 100.0f;
	
	// Maximum speed of the agent.
	UPROPERTY(EditAnywhere, Category = "Sense")
	float AgentSenseRange = 300.0f;

	// Tag used to identify other agents.
	UPROPERTY(EditAnywhere, Category = "Sense")
	FName AgentsTag;

	// Tag used to identify other agents.
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugOtherAgents = false;

	// Tag used to identify other agents.
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bDebugOtherAgents", EditConditionHides = "true"))
	FColor DebugColor;

	// Tag used to identify other agents.
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bDebugOtherAgents", EditConditionHides = "true"))
	float DebugBoxSize;

protected:

	// Defines configuration used to detect other agents and determine if the agent becomes a follow or a seeker.
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayAfter = "AgentsTag"))
	bool bForceLeadership = false;

	// Defines configuration used to detect other agents and determine if the agent becomes a follow or a seeker.
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (EditCondition = "!bForceLeadership", EditConditionHides = "true"))
	FSearchParameters LeaderSearchParameters;

protected:

	UPROPERTY(EditAnywhere, Category = "Seeking", meta = (DisplayAfter = "ChaseConfig"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> SeekingBehaviours;
	
	UPROPERTY(EditAnywhere, Category = "Flocking", meta = (DisplayAfter = "SeekingBehaviours"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> FlockingBehaviours;
	
private:

	TArray<uint32> NearbyAgentIndices;
	
	FWeakActorPtr ChaseTarget;
	TWeakObjectPtr<USpatialGridSubsystem> GridSubsystem;
	
	FVector PreviousLocation = FVector::ZeroVector;
	FVector PreviousVelocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
};
