
#pragma once

#include <CoreMinimal.h>

#include "UObject/Object.h"
#include "BaseAutonomousBehaviour.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class AUTONOMOUSAGENTS_API UBaseAutonomousBehaviour : public UObject
{
	GENERATED_BODY()

public:

	float GetInfluence() const
	{
		return InfluenceScale;
	}
	
	void ScaleInfluence(float Scale)
	{
		InfluenceScale = Scale;
	}

	void ResetInfluence()
	{
		InfluenceScale = 1.0f;
	}
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bIsEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Influence Settings", meta = (UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "1000.0"))
	float Influence = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (EditCondition = "bDebug", EditConditionHides = "true"))
	FColor DebugColor = FColor::Black;
	
protected:
	
	float InfluenceScale = 1.0f;
	
};
