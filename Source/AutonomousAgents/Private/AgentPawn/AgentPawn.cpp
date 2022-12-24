// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentPawn/AgentPawn.h"

#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

// Called when the game starts or when spawned
void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();
	
}
