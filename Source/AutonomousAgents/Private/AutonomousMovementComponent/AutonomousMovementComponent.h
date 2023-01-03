// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class UFloatingPawnMovement;
class UAIPerceptionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AUTONOMOUSAGENTS_API UAutonomousMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAutonomousMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget);

	bool IsSeeker() const;
	
private:

	virtual void PerformChase();
	
	virtual void ApplyCohesion();

	virtual void ApplySeparation();
 
	void SenseOtherAgents();
	
	bool IsAgentInSpecifiedViewCone(const FVector& OtherAgentLocation, float Radius, float HalfFOV) const;
	
	bool CanAgentBecomeSeeker() const;
	
	void ApplyInput();

protected:

#pragma region Cohesion
	UPROPERTY(EditAnywhere, Category = "Cohesion")
	float CohesionBias = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Alignment", meta = (DisplayAfter="CohesionBias"))
	float AlignmentBias = 1.0f;
#pragma endregion

#pragma region Separation AI Sight Config
	UPROPERTY(EditAnywhere, Category = "Separation", meta = (DisplayAfter="AlignmentBias"))
	float SeparationSightRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Separation", meta = (DisplayAfter="AlignmentBias"))
	float SeparationSightHalfFOV = 120.0f;

	UPROPERTY(EditAnywhere, Category = "Separation", meta = (DisplayAfter="AlignmentBias"))
	float SeparationBias = 1.0f;
#pragma endregion

protected:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugCohesion = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugSeparation = false;
	
private:

	FVector MovementInput;

	TArray<AActor*> NearbyAgents;
	
	TWeakObjectPtr<AActor> ChaseTarget;
	TWeakObjectPtr<UFloatingPawnMovement> MovementComponent;
	TWeakObjectPtr<UAIPerceptionComponent> PerceptionComponent;

	bool bIsSeeker = false;
};
