#include "Subsystems/SpatialGridSubsystem.h"

void USpatialGridSubsystem::InitializeGrid(const UGridConfiguration* NewConfiguration)
{
	checkf(NewConfiguration, TEXT("NewConfiguration cannot be null."));
	
	GridParameters = NewConfiguration;
	
	NumBlocks = GridParameters->Resolution; 
	GridAgents.Reserve(GBlockSize * GBitRowSize);

	RowBlocks.Init(FBitBlock(), NumBlocks);
	ColumnBlocks.Init(FBitBlock(), NumBlocks);

	if(GridParameters->bDrawGrid)
	{
		DrawGrid();
	}
}

void USpatialGridSubsystem::Update()
{
	UpdateGrid();
}

void USpatialGridSubsystem::RegisterAgent(const UAgent* NewAgentData)
{
	GridAgents.AddUnique(NewAgentData);
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
				TryDrawCell(CurrentGridLocation);
				
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

void USpatialGridSubsystem::UpdateGrid()
{
	// This function, loops through all registered agents,
	// fetches their world-space locations and maps them to a location on the Grid.
	// Finally it writes new bits into the bit-blocks corresponding to said grid location,
	// to record their presence in a particular row and column.
	
	ResetBlocks();

	for(int AgentIndex = 0; AgentIndex < GridAgents.Num(); ++AgentIndex)
	{
		if(AgentIndex >= GBlockSize * GBitRowSize) break;

		const UAgent* Agent = GridAgents[AgentIndex];
		
		// Find array indices
		FGridCellLocation GridLocation;
		if(!ConvertWorldToGridLocation(Agent->Location, GridLocation))
		{
			continue;
		}
		
		// Create AdditiveMask
		const uint32 BlockLevel = AgentIndex / GBitRowSize;
		const uint32 BitLocation = AgentIndex % GBitRowSize;
		const uint64 AdditiveMask = static_cast<uint64>(1) << BitLocation;

		//const int NewIndex = BlockLevel * GBlockSize + BitLocation;
		//check(NewIndex < GridAgents.Num());
		
		// Apply AdditiveMask
		RowBlocks[GridLocation.X][BlockLevel] |= AdditiveMask;
		ColumnBlocks[GridLocation.Y][BlockLevel] |= AdditiveMask;
	}
}

void USpatialGridSubsystem::GetIndicesInGridLocation(const FGridCellLocation& GridLocation, TArray<int>& Out_Indices) const
{
	if(!IsValidGridLocation(GridLocation))
	{
		return;
	}

	Out_Indices.Reset();
	for(uint32 BlockLevel = 0; BlockLevel < GBlockSize; ++BlockLevel)
	{
		const uint64 IndicesInThisBlock = RowBlocks[GridLocation.X][BlockLevel] & ColumnBlocks[GridLocation.Y][BlockLevel];

		for(int BitLocation = 0; BitLocation < GBitRowSize; ++BitLocation)
		{
			const uint64 FilteredBlock = IndicesInThisBlock & (static_cast<uint64>(1) << BitLocation);
			if(FilteredBlock != 0)
			{
				Out_Indices.Add(BlockLevel * GBitRowSize + BitLocation);
			}
		}
	}
}

bool USpatialGridSubsystem::ConvertWorldToGridLocation(FVector WorldLocation, FGridCellLocation& Out_GridLocation) const
{
	if(!GridParameters || !IsValidWorldLocation(WorldLocation))
	{
		return false;
	}

	const float CellSize = GridParameters->Range.Size<float>() / GridParameters->Resolution;

	WorldLocation.X -= GridParameters->Range.GetLowerBoundValue();
	WorldLocation.Y -= GridParameters->Range.GetLowerBoundValue();

	Out_GridLocation.X = FMath::Clamp(WorldLocation.X / CellSize, 0, NumBlocks - 1);
	Out_GridLocation.Y = FMath::Clamp(WorldLocation.Y / CellSize, 0, NumBlocks - 1);
	
	return true;
}

bool USpatialGridSubsystem::ConvertGridToWorldLocation(const FGridCellLocation& GridLocation, FVector& Out_WorldLocation) const
{
	if(!IsValidGridLocation(GridLocation))
	{
		return false;
	}

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
	return GridParameters->Range.Contains(WorldLocation.X) && GridParameters->Range.Contains(WorldLocation.Y);
}

bool USpatialGridSubsystem::IsValidGridLocation(const FGridCellLocation& GridLocation) const
{
	return RowBlocks.IsValidIndex(GridLocation.X) && ColumnBlocks.IsValidIndex(GridLocation.Y);
}

void USpatialGridSubsystem::ResetBlocks()
{
	for(uint32 BlockIndex = 0; BlockIndex < NumBlocks; ++BlockIndex)
	{
		for(int BlockLevel = 0; BlockLevel < GBlockSize; ++BlockLevel)
		{
			RowBlocks[BlockIndex][BlockLevel] = 0;
			ColumnBlocks[BlockIndex][BlockLevel] = 0;
		}
	}
}

void USpatialGridSubsystem::DrawGrid() const
{
	const float CellWidth = GridParameters->Range.Size<float>() / GridParameters->Resolution;

	float VariableCoordinate = GridParameters->Range.GetLowerBoundValue();
	while(VariableCoordinate <= GridParameters->Range.GetUpperBoundValue())
	{
		FVector LineStart = FVector(GridParameters->Range.GetLowerBoundValue(), VariableCoordinate, 0.0f);
		FVector LineEnd = FVector(GridParameters->Range.GetUpperBoundValue(), VariableCoordinate, 0.0f);
		
		DrawDebugLine(GetWorld(), LineStart, LineEnd, GridParameters->GridColor, true);

		LineStart = FVector(VariableCoordinate, GridParameters->Range.GetLowerBoundValue(), 0.0f);
		LineEnd = FVector(VariableCoordinate, GridParameters->Range.GetUpperBoundValue(), 0.0f);
		
		DrawDebugLine(GetWorld(), LineStart, LineEnd, GridParameters->GridColor, true);

		VariableCoordinate += CellWidth;
	}
}

void USpatialGridSubsystem::TryDrawCell(const FGridCellLocation& GridLocation) const
{
	if(!GridParameters->bDebugLookup || !IsValidGridLocation(GridLocation))
	{
		return;
	}
	
	FVector WorldLocation;
	ConvertGridToWorldLocation(GridLocation, WorldLocation);

	DrawDebugBox(GetWorld(),  WorldLocation, GridParameters->LookupBoxSize * FVector::One(), GridParameters->LookupBoxColor);
}
