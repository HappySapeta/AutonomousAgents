// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/CommonTypes.h"
#include "Components/ActorComponent.h"
#include "SpatialGridComponent.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AUTONOMOUSAGENTS_API USpatialGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	typedef TArray<uint64, TInlineAllocator<BLOCK_SIZE>> FBitBlock;
	
public:	
	// Sets default values for this component's properties
	USpatialGridComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PutActorIntoGrid(const FWeakActorPtr& Actor);

	void GetActorNearLocation(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const;
	
private:

	void CreateGrid();

	void UpdateGrid();

	void GetActorsInCell(const FCellLocation& CellLocation, TArray<uint32>& Indices) const;

	FCellLocation ConvertCoordinatesToCellLocation(FVector Coordinates) const;

	bool IsValidCoordinate(const FVector& Coordinate) const;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	uint32 MaxActors = 100;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	uint32 GridResolution = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	FFloatRange XRange = FFloatRange(0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Grid Setup", meta = (UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100"))
	FFloatRange YRange = FFloatRange(0.0f, 0.0f);
	
private:

	FActorArray GridActors;

	TArray<FBitBlock> XBlocks;
	TArray<FBitBlock> YBlocks;
};
