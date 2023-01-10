// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SpatialGridSubsystem.h"

void USpatialGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitGrid(NewObject<UGridParameters>());
}

void USpatialGridSubsystem::InitializeBlocks()
{
	XBlocks.Init(FBitBlock(), GridParameters->Resolution);
	for(FBitBlock& Block : XBlocks)
	{
		Block.Init(0, BLOCK_SIZE);
	}
	
	YBlocks.Init(FBitBlock(), GridParameters->Resolution);
	for(FBitBlock& Block : YBlocks)
	{
		Block.Init(0, BLOCK_SIZE);
	}
}

void USpatialGridSubsystem::InitGrid(UGridParameters* Parameters)
{
	if(Parameters == nullptr) return;
	
	GridParameters = Parameters;
	
	GridActors.Reserve(GridParameters->MaxActors);
	InitializeBlocks();
}

void USpatialGridSubsystem::Update()
{
	UpdateGrid();
	DebugGrid();
}

void USpatialGridSubsystem::PutActorIntoGrid(const FWeakActorPtr& Actor)
{
	if(GridActors.AddUnique(Actor) >= 0)
	{
		OnActorPresenceUpdatedEvent.Broadcast(Actor.Get());
	}
}

void USpatialGridSubsystem::GetActorNearLocation(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const
{
	if(!IsValidCoordinate(Location)) return;

	Out_ActorIndices.Reset();
	
	const uint32 Reach = FMath::FloorToInt32(Radius / GridParameters->Resolution);
	const FCellLocation& OriginCellLocation = ConvertCoordinatesToCellLocation(Location);
	
	int XIndex = OriginCellLocation.X - Reach;
	int YIndex = OriginCellLocation.Y - Reach;

	while(XIndex < static_cast<int>(OriginCellLocation.X + Reach))
	{
		if(!XBlocks.IsValidIndex(XIndex))
		{
			XIndex += 1;
			continue;
		}
		
		while(YIndex < static_cast<int>(OriginCellLocation.Y + Reach))
		{
			if(YBlocks.IsValidIndex(YIndex))
			{
				TArray<uint32> IndicesInCell;
				GetActorsInCell(FCellLocation(XIndex, YIndex), IndicesInCell);
				Out_ActorIndices.Append(IndicesInCell);
			}
			
			YIndex += 1;
		}
		YIndex = 0;
		XIndex += 1;
	}
}

void USpatialGridSubsystem::DrawGrid() const
{
	const float CellWidth = (GridParameters->XRange.GetUpperBoundValue() - GridParameters->XRange.GetLowerBoundValue()) / GridParameters->Resolution;
	
	FVector StartLocation(GridParameters->XRange.GetLowerBoundValue(), GridParameters->YRange.GetLowerBoundValue(), 0.0f);
	FVector EndLocation(GridParameters->XRange.GetLowerBoundValue(), GridParameters->YRange.GetUpperBoundValue(), 0.0f);
	
	while(StartLocation.X <= GridParameters->XRange.GetUpperBoundValue())
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, GridParameters->Color, false, GridParameters->Duration, 10, GridParameters->Thickness);
		
		StartLocation.X += CellWidth;
		EndLocation.X += CellWidth;
	}

	StartLocation = FVector(GridParameters->XRange.GetLowerBoundValue(), GridParameters->YRange.GetLowerBoundValue(), 0.0f);
	EndLocation = FVector(GridParameters->XRange.GetUpperBoundValue(), GridParameters->YRange.GetLowerBoundValue(), 0.0f);
	
	while(StartLocation.Y <= GridParameters->YRange.GetUpperBoundValue())
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, GridParameters->Color, false, GridParameters->Duration, 11, GridParameters->Thickness);
		
		StartLocation.Y += CellWidth;
		EndLocation.Y += CellWidth;
	}
}

void USpatialGridSubsystem::DebugGrid() const
{
	if(!GridParameters->bDebug) return;
	
	DrawGrid();

	const float XRangeLength = GridParameters->XRange.GetUpperBoundValue() - GridParameters->XRange.GetLowerBoundValue();
	
	FVector LabelLocation = FVector(GridParameters->XRange.GetLowerBoundValue(), GridParameters->XRange.GetLowerBoundValue(), 0.0f);
	for(const FBitBlock& Block : XBlocks)
	{
		TArray<uint32> ActorIndices;
		GetIndicesFromBlock(Block, ActorIndices);
		DrawActors(LabelLocation, ActorIndices);
		
		LabelLocation.X += XRangeLength / (GridParameters->Resolution * 0.5f);
	}
}

void USpatialGridSubsystem::DrawActors(const FVector& Anchor, TArray<uint32> ActorIndices) const
{
	for(const uint32 Index : ActorIndices)
	{
		if(!GridActors.IsValidIndex(Index)) continue;
		DrawDebugLine(GetWorld(), Anchor, GridActors[Index]->GetActorLocation(), FColor::Blue, false, 0.05f);
	}
}

void USpatialGridSubsystem::GetIndicesFromBlock(const FBitBlock& Block, TArray<uint32>& Out_Indices)
{
	for(int BlockIndex = 0; BlockIndex < Block.Num(); ++BlockIndex)
	{
		TArray<uint32> MaskIndices;
		GetIndicesFromMask(Block[BlockIndex], BlockIndex * 64, MaskIndices);

		Out_Indices.Append(MaskIndices);
	}
}

void USpatialGridSubsystem::GetIndicesFromMask(const uint64 BitMask, uint32 Offset, TArray<uint32>& Out_Indices)
{
	Out_Indices.Reset();
	Out_Indices.Init(0, 64);
	
	for(int32 BitLocation = 0; BitLocation < 64; ++BitLocation)
	{
		const uint64 MaskFilter = static_cast<uint64>(1) << BitLocation;
		Out_Indices.Add(BitMask & MaskFilter);
	}
}

void USpatialGridSubsystem::GetActorsInCell(const FCellLocation& CellLocation, TArray<uint32>& Indices) const
{
	if(!XBlocks.IsValidIndex(CellLocation.X) || !YBlocks.IsValidIndex(CellLocation.Y)) return;

	Indices.Reset();
	
	for(uint32 BlockLevel = 0; BlockLevel < BLOCK_SIZE; ++BlockLevel)
	{
		for(uint32 BitLocation = 0; BitLocation < 64; ++BitLocation)
		{
			const uint64 BlockFilter = static_cast<uint64>(1) << BitLocation;
			const bool bIsActorPresentInX = XBlocks[CellLocation.X][BlockLevel] & BlockFilter;
			const bool bIsActorPresentInY = YBlocks[CellLocation.Y][BlockLevel] & BlockFilter;
			
			if(bIsActorPresentInX && bIsActorPresentInY)
			{
				Indices.Add(BlockLevel * BitLocation);
			}
		}
	}
}

void USpatialGridSubsystem::ResetBlocks()
{
	for(FBitBlock& Block : XBlocks)
	{
		for(int i = 0; i < Block.Num(); ++i)
		{
			Block[i] = 0;
		}
	}

	for(FBitBlock& Block : YBlocks)
	{
		for(int i = 0; i < Block.Num(); ++i)
		{
			Block[i] = 0;
		}
	}
}

void USpatialGridSubsystem::UpdateGrid()
{
	ResetBlocks();
	
	for(int ActorIndex = 0; ActorIndex < GridActors.Num(); ++ActorIndex)
	{
		if(ActorIndex >= BLOCK_SIZE * BITMASK_LENGTH) break;

		const FWeakActorPtr& Actor = GridActors[ActorIndex];
		if(!Actor.IsValid()) continue;

		const FVector& ActorLocation = Actor->GetActorLocation();
		if(!IsValidCoordinate(ActorLocation)) continue;
		
		// Find array indices
		const FCellLocation& CellLocation = ConvertCoordinatesToCellLocation(ActorLocation);

		// Find block indices
		const uint32 BlockLevel = ActorIndex / 64;
		const uint32 BlockLocation = ActorIndex % 64;

		// Create bitmask
		const uint64 BlockFilter = static_cast<uint64>(1) << BlockLocation;

		// Apply bitmask on XBlocks
		XBlocks[CellLocation.X][BlockLevel] |= BlockFilter;

		// Apply bitmask on YBlocks
		YBlocks[CellLocation.Y][BlockLevel] |= BlockFilter;
	}
}

FCellLocation USpatialGridSubsystem::ConvertCoordinatesToCellLocation(FVector Coordinates) const
{
	Coordinates.X = FMath::Clamp(Coordinates.X, GridParameters->XRange.GetLowerBoundValue(), GridParameters->XRange.GetUpperBoundValue());
	Coordinates.Y = FMath::Clamp(Coordinates.Y, GridParameters->YRange.GetLowerBoundValue(), GridParameters->YRange.GetUpperBoundValue());

	// Normalize
	Coordinates.X = (Coordinates.X - GridParameters->XRange.GetLowerBoundValue()) / (GridParameters->XRange.GetUpperBoundValue() - GridParameters->XRange.GetLowerBoundValue());
	Coordinates.Y = (Coordinates.Y - GridParameters->YRange.GetLowerBoundValue()) / (GridParameters->YRange.GetUpperBoundValue() - GridParameters->YRange.GetLowerBoundValue());

	// Find array indices
	FCellLocation CellLocation;
	CellLocation.X = FMath::FloorToInt32(Coordinates.X * (GridParameters->Resolution - 1));
	CellLocation.Y = FMath::FloorToInt32(Coordinates.Y * (GridParameters->Resolution - 1));

	return CellLocation;
}

bool USpatialGridSubsystem::IsValidCoordinate(const FVector& Coordinate) const
{
	return GridParameters->XRange.Contains(Coordinate.X) && GridParameters->YRange.Contains(Coordinate.Y);
}
