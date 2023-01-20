
#include "Core/AgentPawn.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AAgentPawn::AAgentPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(SceneComponent);
}

FVector AAgentPawn::GetVelocity() const
{
	return CurrentVelocity;
}

void AAgentPawn::SetData(const FAgentData* Data)
{
	AgentData = Data;
}

void AAgentPawn::BeginPlay()
{
	Super::BeginPlay();
	PreviousLocation = GetActorLocation();
}

void AAgentPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//CalculateCurrentVelocity(DeltaSeconds);
	if(AgentData)
	{
		CurrentVelocity = AgentData->Velocity;
		SetActorLocation(AgentData->Location);
		AlignActorToVelocity(DeltaSeconds);
	}
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
