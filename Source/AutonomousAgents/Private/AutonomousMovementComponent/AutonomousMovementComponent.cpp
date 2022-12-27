// Fill out your copyright notice in the Description page of Project Settings.

#include "AutonomousMovementComponent.h"

#include <GameFramework/FloatingPawnMovement.h>

// Sets default values for this component's properties
UAutonomousMovementComponent::UAutonomousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAutonomousMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Parent = GetOwner();
	if(Parent.IsValid())
	{
		MovementComponent = Cast<UFloatingPawnMovement>(Parent->FindComponentByClass(UFloatingPawnMovement::StaticClass()));
	}
}

// Called every frame
void UAutonomousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	PerformChase();
}

void UAutonomousMovementComponent::PerformChase()
{
	if(!Parent.IsValid() || !MovementComponent.IsValid()) return;
	if(!ChaseTarget.IsValid()) return;
	
	FVector DesiredVelocity = ChaseTarget->GetActorLocation() - Parent->GetActorLocation();
	DesiredVelocity.Normalize();
	DesiredVelocity *= MovementComponent->GetMaxSpeed();

	const FVector& SteerInput = DesiredVelocity - MovementComponent->Velocity;

	DrawDebugLine(GetWorld(), Parent->GetActorLocation(), Parent->GetActorLocation() + SteerInput.GetSafeNormal(), FColor::Red, false, 0.05f, 0, 5.0f);
	MovementComponent->AddInputVector(SteerInput);
}

void UAutonomousMovementComponent::SetChaseTarget(const TWeakObjectPtr<AActor>& NewTarget)
{
	if(NewTarget.IsValid() && ChaseTarget != NewTarget)
	{
		ChaseTarget = NewTarget;
	}
}
