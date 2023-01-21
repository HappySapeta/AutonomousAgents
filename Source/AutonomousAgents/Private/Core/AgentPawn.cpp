
#include "Core/AgentPawn.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(SceneComponent);
}

void AAgentPawn::AlignActorToVelocity(const FVector& Velocity, float DeltaTime)
{
	const FVector& LookAtDirection = Velocity.GetSafeNormal();
	const FRotator& TargetRotation = UKismetMathLibrary::MakeRotFromX(LookAtDirection);
	
	const FRotator& DeltaRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, VelocityAlignmentSpeed);
	
	SetActorRotation(DeltaRotation);
}
