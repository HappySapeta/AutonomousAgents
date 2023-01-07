#pragma once

#include <CoreMinimal.h>
#include "FSense_Config.generated.h"

USTRUCT(BlueprintType)
struct FSense_Config
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Search Settings", meta = (UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "1000.0"))
	FFloatRange SearchRadius = FFloatRange(0.0f, 100.0f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Search Zone Settings", meta = (UIMin = "0.0", UIMax = "180.0", ClampMin = "0.0", ClampMax = "180.0"))
	float FOVHalfAngle = 45.0f;
};
