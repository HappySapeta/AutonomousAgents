// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/CommonTypes.h"
#include "Common/GridParameters.h"
#include "Math/MathFwd.h"
#include "SpatialGridSubsystem.generated.h"

#define BITMASK_LENGTH 64

USTRUCT()
struct FGridLocation
{
	GENERATED_BODY()
	
	FGridLocation()
		: X(0), Y(0)
	{}
	
	FGridLocation(const int XIndex, const int YIndex)
		: X(XIndex), Y(YIndex)
	{}
	
	int X;
	int Y;
};

UCLASS()
class AUTONOMOUSAGENTS_API USpatialGridSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorPresenceUpdatedEvent, AActor*, UpdatedActor);

public:

	UFUNCTION(BlueprintCallable)
	void InitializeGrid(UGridParameters* Parameters);

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void Update();

	void RegisterActor(const FWeakActorPtr& Actor);
	
	void GetActorIndicesInRegion(const FVector& Location, const float Radius, TArray<int>& Out_ActorIndices) const;

	void GetAllActors(FActorArray& Actors) const; 
	
	void DrawGrid() const;
	
	void DrawCell(const FGridLocation& GridLocation) const;

private:
	
	void UpdateGrid();

	void GetIndicesInGridLocation(const FGridLocation& GridLocation, TArray<int>& Out_Indices) const;
	
	void ResetBlocks();

	bool ConvertWorldToGridLocation(FVector WorldLocation, FGridLocation& Out_GridLocation) const;

	bool ConvertGridToWorldLocation(FGridLocation GridLocation, FVector& Out_WorldLocation) const;

	bool IsValidWorldLocation(const FVector& WorldLocation) const;
	
	bool IsValidGridLocation(const FGridLocation& GridLocation) const;

public:

	FOnActorPresenceUpdatedEvent OnActorPresenceUpdatedEvent;

private:

	int NumBlocks;
	
	FActorArray GridActors;

	UPROPERTY(Transient)
	UGridParameters* GridParameters;

	TArray<uint64> Blocks_X;
	TArray<uint64> Blocks_Y;
};
