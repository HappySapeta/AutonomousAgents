// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "FlockingSense_Config/FSense_Config.h"
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class USphereComponent;

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
	
	void SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget);

protected:

	// 1. Initialize variables.
	virtual void BeginPlay() override;

	// Add Force vector to net Movement Force.
	void AddForce(const FVector& Force);

	// Get all agents that fall in the specified view cone.
	void GetAgentsInView(float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle, TArray<TWeakObjectPtr<AActor>>& AgentsInView) const;

	// Check if a point in 3D space falls in a cone defined by its radii and FOV Half-angle.
	bool IsPointInFOV(const FVector& OtherAgentLocation, float MinimumSearchRadius, float MaximumSearchRadius, float HalfFOV) const;

	// Is the agent not surrounded by other agents.
	bool IsAgentLonely() const;

private:

	// Directly seek the chase target.
	virtual void PerformChaseTarget();

	// Move closer to flock
	virtual void PerformFlockCohesion();

	// Move away from other agents.
	virtual void PerformFlockSeparation();

	// Move along the direction of the flock.
	virtual void PerformFlockAlignment();

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
	FSense_Config ChaseConfig;

	// Toggle cohesion.
	UPROPERTY(EditAnywhere, Category = "Cohesion Settings", meta = (DisplayAfter = "ChaseConfig"))
	bool bCohesionEnabled = false;

	// Defines configuration used to detect other agents toward whom this agent will move.
	UPROPERTY(EditAnywhere, Category = "Cohesion Settings",  meta = (EditCondition = "bCohesionEnabled", EditConditionHides = "true", DisplayAfter = "bCohesionEnabled"))
	FSense_Config CohesionConfig;

	// Toggle separation.
	UPROPERTY(EditAnywhere, Category = "Separation Settings", meta = (DisplayAfter = "CohesionConfig"))
	bool bSeparationEnabled = false;

	// Defines configuration used to detect other agents that this agent must avoid. Typically it defines a wider and shorter FOV.
	UPROPERTY(EditAnywhere, Category = "Separation Settings",  meta = (EditCondition = "bSeparationEnabled", EditConditionHides = "true", DisplayAfter = "bSeparationEnabled"))
	FSense_Config SeparationConfig;

	// Toggle alignment.
	UPROPERTY(EditAnywhere, Category = "Alignment Settings", meta = (DisplayAfter = "SeparationConfig"))
	bool bAlignmentEnabled = false;

	// Defines configuration used to detect other agents in whose direction this agent must move.
	UPROPERTY(EditAnywhere, Category = "Alignment Settings",  meta = (EditCondition = "bAlignmentEnabled", EditConditionHides = "true", DisplayAfter = "bAlignmentEnabled"))
	FSense_Config AlignmentConfig;
	
private:

	// Other agents in the vicinity.
	UPROPERTY(Transient)
	TArray<AActor*> SensedAgents;
	
	TWeakObjectPtr<AActor> ChaseTarget;
	TWeakObjectPtr<USphereComponent> SphereComponent;

	FVector PreviousLocation = FVector::ZeroVector;
	FVector PreviousVelocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
};
