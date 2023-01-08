// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SpatialGridComponent.h"

USpatialGridComponent::USpatialGridComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpatialGridComponent::BeginPlay()
{
	Super::BeginPlay();
	GridActors.Reserve(MaxActors);
}

void USpatialGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USpatialGridComponent::PutActorIntoGrid(const FWeakActorPtr& Actor)
{
	GridActors.AddUnique(Actor);
}

void USpatialGridComponent::CreateGrid()
{
	XBlocks.Reserve(GridResolution);
	YBlocks.Reserve(GridResolution);
}

void USpatialGridComponent::GetActorNearLocation(const FVector& Location, const float Radius, TArray<uint32>& Out_ActorIndices) const
{
	if(!IsValidCoordinate(Location)) return;

	Out_ActorIndices.Reset();
	
	const uint32 Reach = FMath::FloorToInt32(Radius / GridResolution);
	const FCellLocation& OriginCellLocation = ConvertCoordinatesToCellLocation(Location);
	
	uint32 XIndex = OriginCellLocation.X - Reach;
	uint32 YIndex = OriginCellLocation.Y - Reach;

	while(XIndex < OriginCellLocation.X + Reach)
	{
		while(YIndex < OriginCellLocation.Y + Reach)
		{
			if(!XBlocks.IsValidIndex(XIndex) || !YBlocks.IsValidIndex(YIndex))
			{
				continue;
			}

			TArray<uint32> IndicesInCell;
			GetActorsInCell(FCellLocation(XIndex, YIndex), IndicesInCell);
			Out_ActorIndices.Append(IndicesInCell);

			YIndex += 1;
		}
		YIndex = 0;
		XIndex += 1;
	}
}

void USpatialGridComponent::GetActorsInCell(const FCellLocation& CellLocation, TArray<uint32>& Indices) const
{
	if(!XBlocks.IsValidIndex(CellLocation.X) || !YBlocks.IsValidIndex(CellLocation.Y)) return;

	Indices.Reset();
	
	for(uint32 BlockLevel = 0; BlockLevel < BLOCK_SIZE; ++BlockLevel)
	{
		for(uint32 BitLocation = 0; BitLocation < 64; ++BitLocation)
		{
			const uint64 BlockFilter = static_cast<uint64>(1) << BitLocation;
			const bool bIsActorPresentInX = XBlocks[CellLocation.X][BlockLevel] & BlockFilter;
			const bool bIsActorPresentInY = YBlocks[CellLocation.X][BlockLevel] & BlockFilter;
			
			if(bIsActorPresentInX && bIsActorPresentInY)
			{
				Indices.Add(BlockLevel * BitLocation);
			}
		}
	}
}

void USpatialGridComponent::UpdateGrid()
{
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

FCellLocation USpatialGridComponent::ConvertCoordinatesToCellLocation(FVector Coordinates) const
{
	Coordinates.X = FMath::Clamp(Coordinates.X, XRange.GetLowerBoundValue(), XRange.GetUpperBoundValue());
	Coordinates.Y = FMath::Clamp(Coordinates.Y, YRange.GetLowerBoundValue(), YRange.GetUpperBoundValue());

	// Normalize
	Coordinates.X = (Coordinates.X - XRange.GetLowerBoundValue()) / (XRange.GetUpperBoundValue() - XRange.GetLowerBoundValue());
	Coordinates.Y = (Coordinates.Y - YRange.GetLowerBoundValue()) / (YRange.GetUpperBoundValue() - YRange.GetLowerBoundValue());

	// Find array indices
	FCellLocation CellLocation;
	CellLocation.X = FMath::FloorToInt32(Coordinates.X * (GridResolution - 1));
	CellLocation.Y = FMath::FloorToInt32(Coordinates.Y * (GridResolution - 1));

	return CellLocation;
}

bool USpatialGridComponent::IsValidCoordinate(const FVector& Coordinate) const
{
	return XRange.Contains(Coordinate.X) && YRange.Contains(Coordinate.Y);
}
