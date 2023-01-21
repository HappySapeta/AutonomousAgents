
#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "BaseAutonomousBehaviour.h"
#include "Common/AgentData.h"
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

	bool CanOtherAgentAffect(const UAgentData* AffectedAgentData, const UAgentData* OtherAgent) const
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

