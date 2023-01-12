#pragma once

#include <CoreMinimal.h>
#include "GridParameters.generated.h"

UCLASS(Blueprintable)
class UGridParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	uint32 Resolution = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup")
	FFloatRange Range = FFloatRange(-2000.0f, 2000.0f);

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings")
	bool bDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDebug", EditConditionHides = "true"))
	FColor GridColor = FColor::Green;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings")
	bool bDrawDebugBox = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDrawDebugBox", EditConditionHides = "true"))
	FColor DebugBoxColor = FColor::Red;
	
	UPROPERTY(EditDefaultsOnly, Category = "Debug Settings", meta = (EditCondition = "bDrawDebugBox", EditConditionHides = "true"))
	float DebugBoxSize = 10.0f;;
};
