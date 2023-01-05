// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "FlockingSense_Config/FSense_Config.h"
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class USphereComponent;
class UFloatingPawnMovement;
class UAIPerceptionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AUTONOMOUSAGENTS_API UAutonomousMovementComponent : public UActorComponent
{

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDynamicDelegate);
	
	GENERATED_BODY()

public:
	
	UAutonomousMovementComponent();

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget);

protected:

	virtual void BeginPlay() override;

	void AddForce(const FVector& Force);

private:

	virtual void PerformChaseTarget();
	
	virtual void PerformFlockCohesion();

	virtual void PerformFlockSeparation();
	
	void GetAgentsInView(float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle, TArray<TWeakObjectPtr<AActor>>& AgentsInView) const;
	
	bool IsAgentInSpecifiedViewCone(const FVector& OtherAgentLocation, float MinimumSearchRadius, float MaximumSearchRadius, float HalfFOV) const;
	
	bool IsAgentLonely() const;
	
	void UpdateActorLocation(float DeltaTime);

protected:

	UPROPERTY(BlueprintAssignable)
	FSimpleDynamicDelegate SetIsFollowing;

	UPROPERTY(BlueprintAssignable)
	FSimpleDynamicDelegate SetIsChasing;

private:

	UFUNCTION()
	void OnEnterDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExitDetection(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:

	UPROPERTY(EditAnywhere, Category = "Force Settings")
	float MaxSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Force Settings")
	bool bLimitForce = false;
	
	UPROPERTY(EditAnywhere, Category = "Force Settings", meta = (EditCondition = "bLimitForce", EditConditionHides = "true"))
	float MaxForce = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Common")
	FName AgentsTag;

protected:
	
	UPROPERTY(EditAnywhere, Category = "Chase Settings", meta = (DisplayAfter = "AgentsTag"))
	FSense_Config FlockSearchConfig;

	UPROPERTY(EditAnywhere, Category = "Chase Settings", meta = (DisplayAfter = "FlockSearchConfig"))
	float ChaseForce;
	
	UPROPERTY(EditAnywhere, Category = "Cohesion Settings", meta = (DisplayAfter = "ChaseForce"))
	bool bCohesionEnabled = false;

	UPROPERTY(EditAnywhere, Category = "Cohesion Settings",  meta = (EditCondition = "bCohesionEnabled", EditConditionHides = "true", DisplayAfter = "bCohesionEnabled"))
	FSense_Config CohesionConfig;

	UPROPERTY(EditAnywhere, Category = "Cohesion Settings",  meta = (EditCondition = "bCohesionEnabled", EditConditionHides = "true", DisplayAfter = "CohesionConfig"))
	float CohesionForce;
	
	UPROPERTY(EditAnywhere, Category = "Separation Settings", meta = (DisplayAfter = "CohesionForce"))
	bool bSeparationEnabled = false;

	UPROPERTY(EditAnywhere, Category = "Separation Settings",  meta = (EditCondition = "bSeparationEnabled", EditConditionHides = "true", DisplayAfter = "bSeparationEnabled"))
	FSense_Config SeparationConfig;

	UPROPERTY(EditAnywhere, Category = "Separation Settings",  meta = (EditCondition = "bSeparationEnabled", EditConditionHides = "true", DisplayAfter = "SeparationConfig"))
	float SeparationForce;
	
private:
	
	UPROPERTY(Transient)
	TArray<AActor*> SensedAgents;
	
	TWeakObjectPtr<AActor> ChaseTarget;
	TWeakObjectPtr<USphereComponent> SphereComponent;

	FVector PreviousLocation = FVector::ZeroVector;
	FVector PreviousVelocity = FVector::ZeroVector;
	FVector MovementForce = FVector::ZeroVector;
};
