// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentPawn/AgentPawn.h"

#include "AutonomousMovementComponent/AutonomousMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Perception/AIPerceptionComponent.h"

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AutonomousMovement = CreateDefaultSubobject<UAutonomousMovementComponent>(TEXT("AutonomousMovementComponent"));
}

// Called when the game starts or when spawned
void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();
	
}
