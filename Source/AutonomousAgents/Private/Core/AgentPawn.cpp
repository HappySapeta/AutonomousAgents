
#include "Core/AgentPawn.h"
#include "Components/AutonomousMovementComponent.h"

#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/SphereComponent.h>

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(SceneComponent);
	
	AutonomousMovement = CreateDefaultSubobject<UAutonomousMovementComponent>(TEXT("AutonomousMovementComponent"));
}

void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();

	// TODO : Delegate target finding task to AIController.
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

	PreviousLocation = GetActorLocation();
}

void AAgentPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CalculateCurrentVelocity(DeltaSeconds);
	AlignActorToVelocity(DeltaSeconds);
}

void AAgentPawn::AlignActorToVelocity(float DeltaSeconds)
{
	const FVector& LookAtDirection = GetVelocity().GetSafeNormal();
	const FRotator& TargetRotation = UKismetMathLibrary::MakeRotFromX(LookAtDirection);

	const FRotator& DeltaRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, VelocityAlignmentSpeed);
	
	SetActorRotation(DeltaRotation);
}

void AAgentPawn::CalculateCurrentVelocity(float DeltaSeconds)
{
	const FVector& CurrentLocation = GetActorLocation();
	
	CurrentVelocity = (CurrentLocation - PreviousLocation) / DeltaSeconds;
	PreviousLocation = CurrentLocation;
}

FVector AAgentPawn::GetVelocity() const
{
	return CurrentVelocity;
}
