// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>

#include "AgentPawn.generated.h"

class USphereComponent;
class UFloatingPawnMovement;
// Forward declarations
class UAIPerceptionComponent;
class UAutonomousMovementComponent;


UCLASS()
class AAgentPawn : public APawn
{
	GENERATED_BODY()

public:
	
	AAgentPawn();
	
	virtual FVector GetVelocity() const override;
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

private:

	void AlignActorToVelocity(float DeltaSeconds);

	void CalculateCurrentVelocity(float DeltaSeconds);

protected:

	UPROPERTY(EditAnywhere, Category = "Target")
	FName ChaseTargetTag;

	UPROPERTY(EditDefaultsOnly, Category = "Rotation", meta = (ClampMin = "0.0", ClampMax = "100.0", RangeMin = "0.0", RangeMax = "100.0"))
	float VelocityAlignmentSpeed = 1.0f;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement", DuplicateTransient)
	TObjectPtr<UAutonomousMovementComponent> AutonomousMovement;

	UPROPERTY(EditAnywhere, Category = "Detection")
	TObjectPtr<USphereComponent> SphereComponent;
	
private:

	FVector CurrentVelocity = FVector::ZeroVector;
	FVector PreviousLocation;
};

