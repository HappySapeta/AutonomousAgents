// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "BaseAutonomousBehaviour.h"
#include "Common/Utility.h"
#include "Common/CommonTypes.h"
#include "Common/FSearchParameters.h"
#include "BaseFlockingBehaviour.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AUTONOMOUSAGENTS_API UBaseFlockingBehaviour : public UBaseAutonomousBehaviour
{
	GENERATED_BODY()

protected:
	
	// Get all agents that fall in the specified view cone.
	void GetAgentsInView(const FWeakActorPtr& ViewingActor, const FActorArray& FromAgents, FActorArray& Out_AgentsInView) const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bIsEnabled", EditConditionHides = "true"))
	FSearchParameters SearchConfig;
};

inline void UBaseFlockingBehaviour::GetAgentsInView(const FWeakActorPtr& ViewingActor, const FActorArray& FromAgents, FActorArray& Out_AgentsInView) const
{
	if (!ViewingActor.IsValid()) return;

	Out_AgentsInView.Reset();
	for (const FWeakActorPtr& Agent : FromAgents)
	{
		if (Agent.IsValid() && Utility::IsPointInFOV(
			ViewingActor->GetActorLocation(), ViewingActor->GetActorForwardVector(), Agent->GetActorLocation(),
			SearchConfig.SearchRadius.GetLowerBoundValue(), SearchConfig.SearchRadius.GetUpperBoundValue(), SearchConfig.FOVHalfAngle))
		{
			Out_AgentsInView.Add(Agent);
		}
	}
}
