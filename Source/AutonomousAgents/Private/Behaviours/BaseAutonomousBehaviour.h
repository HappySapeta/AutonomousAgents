// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "DataTypes/CommonTypes.h"
#include "DataTypes/FSense_Config.h"
#include "BaseAutonomousBehaviour.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AUTONOMOUSAGENTS_API UBaseAutonomousBehaviour : public UObject
{
	GENERATED_BODY()

protected:
	
	// Get all agents that fall in the specified view cone.
	void GetAgentsInView(const FWeakActorPtr& ViewingActor, const FActorArray& FromAgents, FActorArray& Out_AgentsInView) const;

	// Check if a point in 3D space falls in a cone defined by its radii and FOV Half-angle.
	bool IsPointInFOV(const FVector& EyeLocation, const FVector& LookingDirection, const FVector& Point) const;
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bIsEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Influence Settings", meta = (UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "1000.0"))
	float Influence = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bIsEnabled", EditConditionHides = "true"))
	FSense_Config SearchConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDebug = false;
};

inline void UBaseAutonomousBehaviour::GetAgentsInView(const FWeakActorPtr& ViewingActor, const FActorArray& FromAgents, FActorArray& Out_AgentsInView) const
{
	if (!ViewingActor.IsValid()) return;

	Out_AgentsInView.Reset();
	for (const FWeakActorPtr& Agent : FromAgents)
	{
		if (Agent.IsValid() && IsPointInFOV(ViewingActor->GetActorLocation(), ViewingActor->GetActorForwardVector(), Agent->GetActorLocation()))
		{
			Out_AgentsInView.Add(Agent);
		}
	}
}

inline bool UBaseAutonomousBehaviour::IsPointInFOV(const FVector& EyeLocation, const FVector& LookingDirection, const FVector& Point) const
{
	const FVector& PointVector = Point - EyeLocation;
	const float Distance = PointVector.Length();

	const float DotProduct = PointVector.GetSafeNormal().Dot(LookingDirection.GetSafeNormal());
	const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(DotProduct)));
	
	return Distance > SearchConfig.SearchRadius.GetLowerBoundValue() && Distance < SearchConfig.SearchRadius.GetUpperBoundValue() && Angle < SearchConfig.FOVHalfAngle;
}
