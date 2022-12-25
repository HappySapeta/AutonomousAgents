// Fill out your copyright notice in the Description page of Project Settings.

#include "AutonomousMovementComponent.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values for this component's properties
UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	FloatingPawnMovement = Cast<UFloatingPawnMovement>(GetOwner()->FindComponentByClass(UFloatingPawnMovement::StaticClass()));
	PerceptionComponent = Cast<UAIPerceptionComponent>(GetOwner()->FindComponentByClass(UAIPerceptionComponent::StaticClass()));
}


// Called every frame
void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(FloatingPawnMovement.Get());
	check(PerceptionComponent.Get());
	check(Target.Get());
	
	PerformChase();
}

void UAutonomousMovementComponent::PerformChase()
{
	const FVector& CurrentVelocity = FloatingPawnMovement->Velocity;
	const FVector& TargetDirection = (Target->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	
	const FVector& DesiredVelocity = TargetDirection * FloatingPawnMovement->GetMaxSpeed() - CurrentVelocity;

	MovementInput += DesiredVelocity;
}

void UAutonomousMovementComponent::PerformCohesion()
{
}

void UAutonomousMovementComponent::PerformAlignment()
{
}

void UAutonomousMovementComponent::PerformSeparation()
{
}


