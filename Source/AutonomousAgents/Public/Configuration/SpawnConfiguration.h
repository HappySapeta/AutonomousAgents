
#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "SpawnConfiguration.generated.h"

// Forward declarations
class AAgentPawn;

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API USpawnConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	FVector Origin = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Span = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Separation = 100.0f;
	
};
