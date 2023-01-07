// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AgentSpawnerConfig.generated.h"

// Forward declarations
class AAgentPawn;

/**
 * 
 */
UCLASS(Blueprintable)
class AUTONOMOUSAGENTS_API UAgentSpawnerConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	TSubclassOf<AAgentPawn> AgentClass;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	bool bHideFromSceneOutliner = false;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	FVector Origin = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Span = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	float Separation = 100.0f;
	
};
