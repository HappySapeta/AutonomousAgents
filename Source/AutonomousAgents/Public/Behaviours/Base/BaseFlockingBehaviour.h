
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

	bool CanAgentAffect(const FWeakActorPtr& SelfAgent, const FWeakActorPtr& OtherAgent) const
	{
		return Utility::IsPointInFOV(
			SelfAgent->GetActorLocation(), SelfAgent->GetActorForwardVector(), OtherAgent->GetActorLocation(),
			SearchConfig.SearchRadius.GetLowerBoundValue(),
			SearchConfig.SearchRadius.GetUpperBoundValue(),
			SearchConfig.FOVHalfAngle);
	}

protected:
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bIsEnabled", EditConditionHides = "true"))
	FSearchParameters SearchConfig;
};

