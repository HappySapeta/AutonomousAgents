// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SpatialGridSubsystem.h"

void USpatialGridSubsystem::InitializeGrid(const UGridParameters* Parameters)
{
	if(Parameters == nullptr) return;
	
	GridParameters = Parameters;
	
	NumBlocks = GridParameters->Resolution; 
	GridActors.Reserve(BLOCK_SIZE * BIT_ROW_LENGTH);

	RowBlocks.Init(FBitBlock(), NumBlocks);
	ColumnBlocks.Init(FBitBlock(), NumBlocks);
}

void USpatialGridSubsystem::Update()
{
	UpdateGrid();
	DrawGrid();
}

void USpatialGridSubsystem::UpdateGrid()
{
	ResetBlocks();

	for(int ActorIndex = 0; ActorIndex < GridActors.Num(); ++ActorIndex)
	{
		if(ActorIndex >= BLOCK_SIZE * BIT_ROW_LENGTH) break;

		const FWeakActorPtr& Actor = GridActors[ActorIndex];
		if(!Actor.IsValid())
		{
			continue;
		}
		
		// Find array indices
		FGridCellLocation GridLocation;
		if(!ConvertWorldToGridLocation(Actor->GetActorLocation(), GridLocation))
		{
			continue;
		}
		
		// Create AdditiveMask
		const uint32 BlockLevel = ActorIndex / BIT_ROW_LENGTH;
		const uint32 BitLocation = ActorIndex % BIT_ROW_LENGTH;
		const uint64 AdditiveMask = static_cast<uint64>(1) << BitLocation;

		// Apply AdditiveMask
		RowBlocks[GridLocation.X][BlockLevel] |= AdditiveMask;
		ColumnBlocks[GridLocation.Y][BlockLevel] |= AdditiveMask;
	}
}

void USpatialGridSubsystem::RegisterActor(const FWeakActorPtr& Actor)
{
	GridActors.AddUnique(Actor);
}

void USpatialGridSubsystem::SearchActors(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const
{
	if(!GridParameters) return;

	const int Reach = FMath::FloorToInt(Radius / GridParameters->Range.Size<float>() * GridParameters->Resolution);

	FGridCellLocation SearchGridLocation;
	if(!ConvertWorldToGridLocation(Location, SearchGridLocation))
	{
		return;
	}
	
	const FGridCellLocation& StartGridLocation = FGridCellLocation(SearchGridLocation.X - Reach, SearchGridLocation.Y - Reach);
	const FGridCellLocation& EndGridLocation = FGridCellLocation(SearchGridLocation.X + Reach, SearchGridLocation.Y + Reach);

	FGridCellLocation CurrentGridLocation = StartGridLocation;
	while(CurrentGridLocation.X <= EndGridLocation.X)
	{
		while(CurrentGridLocation.Y <= EndGridLocation.Y)
		{
			if(IsValidGridLocation(CurrentGridLocation))
			{
				DrawCell(CurrentGridLocation);
				
				TArray<int> IndicesInThisCell;
				GetIndicesInGridLocation(CurrentGridLocation, IndicesInThisCell);

				Out_ActorIndices.Append(IndicesInThisCell);
			}
			CurrentGridLocation.Y += 1;
		}
		CurrentGridLocation.Y = StartGridLocation.Y;
		CurrentGridLocation.X += 1;
	}
}

const FActorArray* USpatialGridSubsystem::GetActorArray() const
{
	return &GridActors;
}

void USpatialGridSubsystem::GetIndicesInGridLocation(const FGridCellLocation& GridLocation, TArray<int>& Out_Indices) const
{
	if(!IsValidGridLocation(GridLocation)) return;

	Out_Indices.Reset();
	for(uint32 BlockLevel = 0; BlockLevel < BLOCK_SIZE; ++BlockLevel)
	{
		const uint64 IndicesInThisBlock = RowBlocks[GridLocation.X][BlockLevel] & ColumnBlocks[GridLocation.Y][BlockLevel];

		for(int BitLocation = 0; BitLocation < BIT_ROW_LENGTH; ++BitLocation)
		{
			const uint64 FilteredBlock = IndicesInThisBlock & (static_cast<uint64>(1) << BitLocation);
			if(FilteredBlock != 0)
			{
				Out_Indices.Add(BlockLevel * BLOCK_SIZE + BitLocation);
			}
		}
	}
}

bool USpatialGridSubsystem::ConvertWorldToGridLocation(FVector WorldLocation, FGridCellLocation& Out_GridLocation) const
{
	if(!GridParameters || !IsValidWorldLocation(WorldLocation)) return false;

	const float CellSize = GridParameters->Range.Size<float>() / GridParameters->Resolution;

	WorldLocation.X -= GridParameters->Range.GetLowerBoundValue();
	WorldLocation.Y -= GridParameters->Range.GetLowerBoundValue();

	Out_GridLocation.X = FMath::Clamp(WorldLocation.X / CellSize, 0, NumBlocks - 1);
	Out_GridLocation.Y = FMath::Clamp(WorldLocation.Y / CellSize, 0, NumBlocks - 1);
	
	return true;
}

bool USpatialGridSubsystem::ConvertGridToWorldLocation(const FGridCellLocation& GridLocation, FVector& Out_WorldLocation) const
{
	if(!GridParameters || !IsValidGridLocation(GridLocation)) return false;

	const float RangeSize = GridParameters->Range.Size<float>();
	
	// Normalize
	Out_WorldLocation.X = GridLocation.X / static_cast<float>(NumBlocks);
	Out_WorldLocation.Y = GridLocation.Y / static_cast<float>(NumBlocks);

	// Scale
	Out_WorldLocation.X *= RangeSize;
	Out_WorldLocation.Y *= RangeSize;

	// Translate
	const float LowerBoundValue = GridParameters->Range.GetLowerBoundValue();
	const float CellSize = RangeSize / GridParameters->Resolution;
	Out_WorldLocation.X += LowerBoundValue + (CellSize * 0.5f);
	Out_WorldLocation.Y += LowerBoundValue + (CellSize * 0.5f);
	
	return true;
}

bool USpatialGridSubsystem::IsValidWorldLocation(const FVector& WorldLocation) const
{
	if(!GridParameters) return false;
	return GridParameters->Range.Contains(WorldLocation.X) && GridParameters->Range.Contains(WorldLocation.Y);
}

bool USpatialGridSubsystem::IsValidGridLocation(const FGridCellLocation& GridLocation) const
{
	if(!GridParameters) return false;
	return RowBlocks.IsValidIndex(GridLocation.X) && ColumnBlocks.IsValidIndex(GridLocation.Y);
}

void USpatialGridSubsystem::ResetBlocks()
{
	if(!GridParameters) return;
	
	for(uint32 BlockIndex = 0; BlockIndex < NumBlocks; ++BlockIndex)
	{
		for(int BlockLevel = 0; BlockLevel < BLOCK_SIZE; ++BlockLevel)
		{
			RowBlocks[BlockIndex][BlockLevel] = 0;
			ColumnBlocks[BlockIndex][BlockLevel] = 0;
		}
	}
}

void USpatialGridSubsystem::DrawGrid() const
{
	if(!GridParameters) return;
	if(!GridParameters->bDebug) return;

	const float CellWidth = GridParameters->Range.Size<float>() / GridParameters->Resolution;

	float VariableCoordinate = GridParameters->Range.GetLowerBoundValue();
	while(VariableCoordinate <= GridParameters->Range.GetUpperBoundValue())
	{
		FVector LineStart = FVector(GridParameters->Range.GetLowerBoundValue(), VariableCoordinate, 0.0f);
		FVector LineEnd = FVector(GridParameters->Range.GetUpperBoundValue(), VariableCoordinate, 0.0f);
		DrawDebugLine(GetWorld(), LineStart, LineEnd, GridParameters->GridColor);

		LineStart = FVector(VariableCoordinate, GridParameters->Range.GetLowerBoundValue(), 0.0f);
		LineEnd = FVector(VariableCoordinate, GridParameters->Range.GetUpperBoundValue(), 0.0f);
		DrawDebugLine(GetWorld(), LineStart, LineEnd, GridParameters->GridColor);

		VariableCoordinate += CellWidth;
	}
}

void USpatialGridSubsystem::DrawCell(const FGridCellLocation& GridLocation) const
{
	if(!GridParameters) return;
	if(!GridParameters->bDrawDebugBox) return;
	if(!IsValidGridLocation(GridLocation)) return;

	FVector WorldLocation;
	ConvertGridToWorldLocation(GridLocation, WorldLocation);

	DrawDebugBox(GetWorld(),  WorldLocation, GridParameters->DebugBoxSize * FVector::One(), GridParameters->DebugBoxColor);
}
