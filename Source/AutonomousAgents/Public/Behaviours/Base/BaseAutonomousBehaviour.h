// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseAutonomousBehaviour.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AUTONOMOUSAGENTS_API UBaseAutonomousBehaviour : public UObject
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bIsEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Influence Settings", meta = (UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "1000.0"))
	float Influence = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDebug = false;
};
