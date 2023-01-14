
#pragma once

#include "CoreMinimal.h"
#include "Common/CommonTypes.h"
#include "Common/GridParameters.h"
#include "Math/MathFwd.h"
#include "SpatialGridSubsystem.generated.h"

#define BIT_ROW_LENGTH 64
#define BLOCK_SIZE 20

USTRUCT()
struct FGridCellLocation
{
	GENERATED_BODY()
	
	FGridCellLocation()
		: X(0), Y(0)
	{}
	
	FGridCellLocation(const int XIndex, const int YIndex)
		: X(XIndex), Y(YIndex)
	{}
	
	int X;
	int Y;
};

USTRUCT()
struct FBitBlock
{
	GENERATED_BODY();
	
	FBitBlock()
	{
		BitRow.Init(0, BLOCK_SIZE);
	}

	uint64& operator[](const uint32 Index)
	{
		return BitRow[Index];
	}
	
	const uint64& operator[](const uint32 Index) const
	{
		return BitRow[Index];
	}

	TArray<uint64, TInlineAllocator<BLOCK_SIZE>> BitRow;
};

USTRUCT()
struct FBlockArray
{
	GENERATED_BODY();
	
	TArray<FBitBlock> RowBlocks;
	TArray<FBitBlock> ColumnBlocks;
};

UCLASS()
class AUTONOMOUSAGENTS_API USpatialGridSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void InitializeGrid(const UGridParameters* Parameters);
	
	void Update();

	void RegisterActor(const FWeakActorPtr& Actor);
	
	void SearchActors(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const;

	const FActorArray* GetActorArray() const;

public:
	
	void DrawGrid() const;
	
	void DrawCell(const FGridCellLocation& GridLocation) const;

private:
	
	void UpdateGrid();

	void GetIndicesInGridLocation(const FGridCellLocation& GridLocation, TArray<int>& Out_Indices) const;

	bool ConvertWorldToGridLocation(FVector WorldLocation, FGridCellLocation& Out_GridLocation) const;

	bool ConvertGridToWorldLocation(const FGridCellLocation& GridLocation, FVector& Out_WorldLocation) const;

	bool IsValidWorldLocation(const FVector& WorldLocation) const;

	bool IsValidGridLocation(const FGridCellLocation& GridLocation) const;

	void ResetBlocks();

private:

	uint32 NumBlocks;
	
	FActorArray GridActors;

	UPROPERTY(Transient)
	const UGridParameters* GridParameters;

	TArray<FBitBlock> RowBlocks;
	TArray<FBitBlock> ColumnBlocks;
};
