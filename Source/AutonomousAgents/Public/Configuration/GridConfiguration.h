#pragma once

#include <CoreMinimal.h>
#include "GridConfiguration.generated.h"

UCLASS(Blueprintable)
class UGridConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	uint32 Resolution = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup")
	FFloatRange Range = FFloatRange(-2000.0f, 2000.0f);

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings")
	bool bDrawGrid = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDrawGrid", EditConditionHides = "true"))
	FColor GridColor = FColor::Green;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings")
	bool bDebugLookup = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDebugLookup", EditConditionHides = "true"))
	FColor LookupBoxColor = FColor::Red;
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDebugLookup", EditConditionHides = "true"))
	float LookupBoxSize = 10.0f;
};
