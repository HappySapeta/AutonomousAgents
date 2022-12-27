// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentPawn/AgentPawn.h"
#include "AutonomousMovementComponent/AutonomousMovementComponent.h"

#include <GameFramework/FloatingPawnMovement.h>
#include <Kismet/GameplayStatics.h>

#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	AutonomousMovement = CreateDefaultSubobject<UAutonomousMovementComponent>(TEXT("AutonomousMovementComponent"));
}

// Called when the game starts or when spawned
void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();
	if(AutonomousMovement)
	{
		TArray<AActor*> ChaseTargets;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), ChaseTargetTag, ChaseTargets);

		if(ChaseTargets.Num() > 0)
		{
			const int RandomIndex = FMath::RandRange(0, ChaseTargets.Num() - 1);
			AutonomousMovement->SetChaseTarget(ChaseTargets[RandomIndex]);
		}
	}
}

void AAgentPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	AlignActorToVelocity(DeltaSeconds);
}

void AAgentPawn::AlignActorToVelocity(float DeltaSeconds)
{
	const FVector& LookAtDirection = FloatingPawnMovement->Velocity.GetSafeNormal();
	const FRotator& LookAtRotation = UKismetMathLibrary::MakeRotFromX(LookAtDirection);

	const FRotator& DeltaRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), LookAtRotation, DeltaSeconds, VelocityAlignmentSpeed);
	
	SetActorRotation(DeltaRotation);
}
