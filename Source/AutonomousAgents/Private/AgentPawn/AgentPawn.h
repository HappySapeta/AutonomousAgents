// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AgentPawn.generated.h"

// Forward declarations
class UFloatingPawnMovement;


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

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;
};
