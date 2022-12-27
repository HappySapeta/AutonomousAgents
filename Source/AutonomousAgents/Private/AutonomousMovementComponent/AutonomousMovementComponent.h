// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include "AutonomousMovementComponent.generated.h"

// Forward declarations
class UAIPerceptionComponent;
class UFloatingPawnMovement;

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
	
private:

	virtual void PerformChase();

private:

	TWeakObjectPtr<AActor> ChaseTarget;
	TWeakObjectPtr<UFloatingPawnMovement> MovementComponent;
};
