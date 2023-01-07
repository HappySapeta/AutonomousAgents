// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "DataTypes/CommonTypes.h"
#include "DataTypes/FSense_Config.h"
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class UBaseAutonomousBehaviour;
class USphereComponent;

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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDynamicDelegate);
	
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

	// Get all agents that fall in the specified view cone.
	void GetAgentsInView(float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle, FActorArray& AgentsInView) const;

	// Check if a point in 3D space falls in a cone defined by its radii and FOV Half-angle.
	bool IsPointInFOV(const FVector& OtherAgentLocation, float MinimumSearchRadius, float MaximumSearchRadius, float HalfFOV) const;

	// Is the agent not surrounded by other agents.
	bool CanAgentLead() const;

private:

	// Calculate velocity and new location from the net Movement Force.
	virtual void PhysicsUpdate(float DeltaTime);

private:

	// Checks if an actor identifies itself as an agent.
	UFUNCTION()
	void OnEnterDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Check if an agent left the detection sphere.
	UFUNCTION()
	void OnExitDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:

	// Performs cosmetic updates.
	UPROPERTY(BlueprintAssignable)
	FSimpleDynamicDelegate SetIsFollowing;

	// Performs cosmetic updates.
	UPROPERTY(BlueprintAssignable)
	FSimpleDynamicDelegate SetIsChasing;
	
protected:

	// Maximum speed of the agent.
	UPROPERTY(EditAnywhere, Category = "Force Settings")
	float MaxSpeed = 100.0f;

	// Tag used to identify other agents.
	UPROPERTY(EditAnywhere, Category = "Common")
	FName AgentsTag;

protected:

	// Defines configuration used to detect other agents and determine if the agent becomes a follow or a seeker.
	UPROPERTY(EditAnywhere, Category = "Chase Settings", meta = (DisplayAfter = "AgentsTag"))
	FSense_Config LeaderCheckConfig;
	
protected:

	UPROPERTY(EditAnywhere, Category = "Seeking", meta = (DisplayAfter = "ChaseConfig"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> SeekingBehaviours;
	
	UPROPERTY(EditAnywhere, Category = "Flocking", meta = (DisplayAfter = "SeekingBehaviours"))
	TArray<TSubclassOf<UBaseAutonomousBehaviour>> FlockingBehaviours;
	
private:

	// Other agents in the vicinity.
	FActorArray SensedAgents;
	
	FWeakActorPtr ChaseTarget;
	TWeakObjectPtr<USphereComponent> SphereComponent;

	FVector PreviousLocation = FVector::ZeroVector;
	FVector PreviousVelocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
};
