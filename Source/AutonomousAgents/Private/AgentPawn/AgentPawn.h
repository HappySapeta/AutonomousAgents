// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>

#include "AgentPawn.generated.h"

// Forward declarations
class UFloatingPawnMovement;
class UAIPerceptionComponent;
class UAutonomousMovementComponent;


UCLASS()
class AAgentPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAgentPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:

	void AlignActorToVelocity(float DeltaSeconds);
	
protected:

	UPROPERTY(EditAnywhere, Category = "Target")
	FName ChaseTargetTag;

	UPROPERTY(EditDefaultsOnly, Category = "Rotation", meta = (ClampMin = "0.0", ClampMax = "100.0", RangeMin = "0.0", RangeMax = "100.0"))
	float VelocityAlignmentSpeed = 1.0f;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	TObjectPtr<UAutonomousMovementComponent> AutonomousMovement;
};
