#pragma once

#include <CoreMinimal.h>

#include "BaseAutonomousBehaviour.h"
#include "Core/Agent.h"
#include "Common/Utility.h"
#include "Configuration/SearchParameters.h"
#include "BaseFlockingBehaviour.generated.h"

/**
 * Base class for all flocking behaviours.
 * Includes a SearchConfig parameter used to filter nearby agents. 
 */
UCLASS(Abstract)
class AUTONOMOUSAGENTS_API UBaseFlockingBehaviour : public UBaseAutonomousBehaviour
{
	GENERATED_BODY()

protected:

	bool CanOtherAgentAffect(const UAgent* AffectedAgentData, const UAgent* OtherAgent) const
	{
		return Utility::IsPointInFOV(
			AffectedAgentData->Location, AffectedAgentData->GetForwardVector(), OtherAgent->Location,
			SearchConfig.SearchRadius.GetLowerBoundValue(),
			SearchConfig.SearchRadius.GetUpperBoundValue(),
			SearchConfig.FOVHalfAngle);
	}

protected:
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bIsEnabled", EditConditionHides = "true"))
	FSearchParameters SearchConfig;
};

