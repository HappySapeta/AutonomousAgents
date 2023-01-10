// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/CommonTypes.h"
#include "Common/GridParameters.h"
#include "Components/ActorComponent.h"
#include "SpatialGridSubsystem.generated.h"

#define BLOCK_SIZE 20
#define BITMASK_LENGTH 64

USTRUCT()
struct FCellLocation
{
	GENERATED_BODY()

	FCellLocation()
		: X(0), Y(0)
	{}
	
	FCellLocation(const uint32 XIndex, const uint32 YIndex)
		: X(XIndex), Y(YIndex)
	{}
	
	uint32 X;
	uint32 Y;
};

UCLASS()
class AUTONOMOUSAGENTS_API USpatialGridSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorPresenceUpdatedEvent, AActor*, UpdatedActor);
	
public:

	typedef TArray<uint64, TInlineAllocator<BLOCK_SIZE>> FBitBlock;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void InitGrid(UGridParameters* Parameters);

	void Update();

	void PutActorIntoGrid(const FWeakActorPtr& Actor);
	
	void GetActorNearLocation(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const;
	void DrawGrid() const;

	void DebugGrid() const;

private:
	
	void UpdateGrid();

	static void GetIndicesFromBlock(const FBitBlock& Block, TArray<uint32>& Out_Indices);

	static void GetIndicesFromMask(const uint64 BitMask, uint32 Offset, TArray<uint32>& Out_Indices);
	
	void DrawActors(const FVector& Anchor, TArray<uint32> ActorIndices) const;
	
	void GetActorsInCell(const FCellLocation& CellLocation, TArray<uint32>& Indices) const;
	
	void ResetBlocks();

	void InitializeBlocks();

	FCellLocation ConvertCoordinatesToCellLocation(FVector Coordinates) const;

	bool IsValidCoordinate(const FVector& Coordinate) const;

public:

	FOnActorPresenceUpdatedEvent OnActorPresenceUpdatedEvent;

private:

	FActorArray GridActors;

	UPROPERTY(Transient)
	UGridParameters* GridParameters;

	TArray<FBitBlock> XBlocks;
	TArray<FBitBlock> YBlocks;
};
