// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SpatialGridSubsystem.h"

void USpatialGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USpatialGridSubsystem::InitializeGrid(UGridParameters* Parameters)
{
	if(Parameters == nullptr) return;
	
	GridParameters = Parameters;
	
	NumBlocks = GridParameters->Resolution; 
	GridActors.Reserve(BITMASK_LENGTH);

	Blocks_X.Init(static_cast<uint64>(0), NumBlocks);
	Blocks_Y.Init(static_cast<uint64>(0), NumBlocks);
}

void USpatialGridSubsystem::Update()
{
	ResetBlocks();
	UpdateGrid();
	DrawGrid();
}

void USpatialGridSubsystem::UpdateGrid()
{
	for(int ActorIndex = 0; ActorIndex < GridActors.Num(); ++ActorIndex)
	{
		if(ActorIndex >= BITMASK_LENGTH) break;

		const FWeakActorPtr& Actor = GridActors[ActorIndex];
		if(!Actor.IsValid())
		{
			continue;
		}
		
		// Find array indices
		FGridLocation GridLocation;
		if(!ConvertWorldToGridLocation(Actor->GetActorLocation(), GridLocation))
		{
			continue;
		}
		
		// Create bitmask
		const uint64 BitMask = static_cast<uint64>(1) << ActorIndex;

		// Apply bitmask Blocks
		Blocks_X[GridLocation.X] |= BitMask;
		Blocks_Y[GridLocation.Y] |= BitMask;
	}
}

void USpatialGridSubsystem::RegisterActor(const FWeakActorPtr& Actor)
{
	if(GridActors.AddUnique(Actor) >= 0)
	{
		OnActorPresenceUpdatedEvent.Broadcast(Actor.Get());
	}
}

void USpatialGridSubsystem::GetActorIndicesInRegion(const FVector& Location, const float Radius, TArray<int>& Out_ActorIndices) const
{
	if(!GridParameters) return;

	const int Reach = FMath::FloorToInt(Radius / GridParameters->Range.Size<float>() * GridParameters->Resolution);

	FGridLocation SearchGridLocation;
	if(!ConvertWorldToGridLocation(Location, SearchGridLocation))
	{
		return;
	}
	
	Out_ActorIndices.Reset();

	const FGridLocation& StartGridLocation = FGridLocation(SearchGridLocation.X - Reach, SearchGridLocation.Y - Reach);
	const FGridLocation& EndGridLocation = FGridLocation(SearchGridLocation.X + Reach, SearchGridLocation.Y + Reach);

	FGridLocation CurrentGridLocation = StartGridLocation;
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

void USpatialGridSubsystem::GetAllActors(FActorArray& Actors) const
{
	Actors = GridActors;
}

void USpatialGridSubsystem::GetIndicesInGridLocation(const FGridLocation& GridLocation, TArray<int>& Out_Indices) const
{
	if(!IsValidGridLocation(GridLocation)) return;

	Out_Indices.Reset();
	const uint64 IndicesPresentAtLocation = Blocks_X[GridLocation.X] & Blocks_Y[GridLocation.Y];

	for(int BitLocation = 0; BitLocation < BITMASK_LENGTH; ++BitLocation)
	{
		const uint64 FilteredBlock = IndicesPresentAtLocation & (static_cast<uint64>(1) << BitLocation);
		if(FilteredBlock != 0)
		{
			Out_Indices.Add(BitLocation);
		}
	}
}

bool USpatialGridSubsystem::ConvertWorldToGridLocation(FVector WorldLocation, FGridLocation& Out_GridLocation) const
{
	if(!GridParameters || !IsValidWorldLocation(WorldLocation)) return false;

	const float CellSize = GridParameters->Range.Size<float>() / GridParameters->Resolution;

	WorldLocation.X -= GridParameters->Range.GetLowerBoundValue();
	WorldLocation.Y -= GridParameters->Range.GetLowerBoundValue();

	Out_GridLocation.X = FMath::Clamp(WorldLocation.X / CellSize, 0, NumBlocks - 1);
	Out_GridLocation.Y = FMath::Clamp(WorldLocation.Y / CellSize, 0, NumBlocks - 1);
	
	return true;
}

bool USpatialGridSubsystem::ConvertGridToWorldLocation(FGridLocation GridLocation, FVector& Out_WorldLocation) const
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

bool USpatialGridSubsystem::IsValidGridLocation(const FGridLocation& GridLocation) const
{
	if(!GridParameters) return false;
	return Blocks_X.IsValidIndex(GridLocation.X) && Blocks_Y.IsValidIndex(GridLocation.Y);
}

void USpatialGridSubsystem::ResetBlocks()
{
	if(!GridParameters) return;
	
	for(int BlockIndex = 0; BlockIndex < NumBlocks; ++BlockIndex)
	{
		Blocks_X[BlockIndex] = 0;
		Blocks_Y[BlockIndex] = 0;
	}
}

void USpatialGridSubsystem::DrawGrid() const
{
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

void USpatialGridSubsystem::DrawCell(const FGridLocation& GridLocation) const
{
	if(!GridParameters) return;
	if(!GridParameters->bDrawDebugBox) return;
	if(!IsValidGridLocation(GridLocation)) return;

	FVector WorldLocation;
	ConvertGridToWorldLocation(GridLocation, WorldLocation);

	DrawDebugBox(GetWorld(),  WorldLocation, GridParameters->DebugBoxSize * FVector::One(), GridParameters->DebugBoxColor);
}