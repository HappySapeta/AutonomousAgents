#pragma once

#include <CoreMinimal.h>
#include <Math/MathFwd.h>

#include "Core/Agent.h"
#include "Configuration/GridConfiguration.h"
#include "SpatialGridSubsystem.generated.h"

constexpr int GBitRowSize = 64;
constexpr int GBlockSize = 20;

// Represents a location on the grid in terms of Column and Row Indices.
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
	
	int X; // Row Index
	int Y; // Column Index
};

// Wrapper over an Array of 64-bit Integers.
USTRUCT()
struct FBitBlock
{
	GENERATED_BODY();
	
	FBitBlock()
	{
		BitRow.Init(0, GBlockSize);
	}

	uint64& operator[](const uint32 Index)
	{
		return BitRow[Index];
	}
	
	const uint64& operator[](const uint32 Index) const
	{
		return BitRow[Index];
	}

	TArray<uint64, TInlineAllocator<GBlockSize>> BitRow;
};

/**
 * The SpatialGridSubsystem implements an implicit spatial grid
 * that keeps track of the location of entities
 * and provides a fast lookup feature to find entities in a certain region.
 */
UCLASS()
class AUTONOMOUSAGENTS_API USpatialGridSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief Reserves and initializes arrays with 0's.
	 * @param NewConfiguration The configuration UDataAsset that the subsystem must use to get all its configuration information.
	 */
	UFUNCTION(BlueprintCallable)
	void InitializeGrid(const UGridConfiguration* NewConfiguration);

public:

	
	void Update();

	void RegisterAgent(const UAgent* NewAgentData);
	
	void SearchActors(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const;
	
	void DrawGrid() const;
	
	void TryDrawCell(const FGridCellLocation& GridLocation) const;

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

	UPROPERTY(Transient)
	TArray<const UAgent*> GridAgents;

	UPROPERTY(Transient)
	const UGridConfiguration* GridParameters;

	TArray<FBitBlock> RowBlocks;
	TArray<FBitBlock> ColumnBlocks;
};
