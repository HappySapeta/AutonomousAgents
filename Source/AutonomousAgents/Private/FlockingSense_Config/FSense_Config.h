#pragma once

#include <CoreMinimal.h>
#include "FSense_Config.generated.h"

USTRUCT(BlueprintType)
struct FSense_Config
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Search Zone Settings")
	float MaximumSearchRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Search Zone Settings")
	float MinimumSearchRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Search Zone Settings")
	float FOVHalfAngle = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bDebug = false;
};
