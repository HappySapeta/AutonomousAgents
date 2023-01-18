
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
}

void AAgentPawn::SetIsChasing(bool Value)
{
	if(Value)
	{
		IsChasing();
	}
	else
	{
		IsFollowing();
	}
}

void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();
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
	const FVector& LookAtDirection = CurrentVelocity.GetSafeNormal();
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
