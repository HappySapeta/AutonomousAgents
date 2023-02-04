
#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "SpawnConfiguration.generated.h"

// Forward declarations
class AAgentPawn;

/**
 * Spawn Configuration and Visual Settings.
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API USpawnConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:

	// Point in the world around which the agents are spawned
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	FVector Origin = FVector::ZeroVector;

	// Distance across which agents are spawned.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Span = 100.0f;

	// Separation between each spawned agent.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Separation = 100.0f;

	// Visual representation of an agent.
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* AgentMesh;

	// Material applied on the static mesh of the agents.
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UMaterialInstance* Material;
	
};
