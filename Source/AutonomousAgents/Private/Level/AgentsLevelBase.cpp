#include "Level/AgentsLevelBase.h"
#include "Configuration/SpawnConfiguration.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/SimulationSubsystem.h"

AAgentsLevelBase::AAgentsLevelBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAgentsLevelBase::Init(const USpawnConfiguration* NewSpawnConfiguration)
{
	checkf(NewSpawnConfiguration, TEXT("Configuration cannot be null"));
	SpawnConfiguration = NewSpawnConfiguration;
	
	FetchSubsystems();
	CreateInstancedStaticMeshComponent();

	check(SimulationSubsystem);
	
	bInitialized = true;
}

void AAgentsLevelBase::SpawnAgents()
{
	checkf(bInitialized, TEXT("AgentsSandboxLevelScript hasn't been initialized. Did you make a Init call?"));
	
	const float Span = SpawnConfiguration->Span;
	const FVector Origin = SpawnConfiguration->Origin;
	FVector SpawnLocation = Origin - FVector(Span, Span, 0.0f);

	const FVector& LowerBound = Origin - FVector(Span, Span, 0.0f);
	const FVector& UpperBound = Origin + FVector(Span, Span, 0.0f);

	NumAgents = 0;
	while(SpawnLocation.X >= LowerBound.X && SpawnLocation.X <= UpperBound.X)
	{
		while(SpawnLocation.Y >= LowerBound.Y && SpawnLocation.Y <= UpperBound.Y)
		{
			SpawnSingleAgent(SpawnLocation, NumAgents);
			++NumAgents;
			SpawnLocation.Y += SpawnConfiguration->Separation;
		}
		
		SpawnLocation.X += SpawnConfiguration->Separation;
		SpawnLocation.Y = SpawnConfiguration->Origin.Y - Span;
	}

	const FString Message = FString::Printf(TEXT("Simulating %d agents."), NumAgents);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
}

void AAgentsLevelBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SimulationSubsystem->Tick(DeltaSeconds);
	UpdateInstancedMeshes();
}

void AAgentsLevelBase::UpdateInstancedMeshes() const
{
	InstancedStaticMeshComponent->BatchUpdateInstancesTransforms(0, SimulationSubsystem->GetTransforms(), true, false);
	InstancedStaticMeshComponent->BatchUpdateInstancesTransform(NumAgents - 1, 1, SimulationSubsystem->GetTransform(NumAgents - 1), true, true);
}

void AAgentsLevelBase::SpawnSingleAgent(FVector SpawnLocation, const uint32 InstanceIndex) const
{
	const FTransform& Transform = FTransform(FRotator::ZeroRotator, SpawnLocation);
	InstancedStaticMeshComponent->AddInstance(Transform);
	InstancedStaticMeshComponent->SetMaterial(InstanceIndex, SpawnConfiguration->Material);
}

void AAgentsLevelBase::CreateInstancedStaticMeshComponent()
{
	InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
	InstancedStaticMeshComponent->RegisterComponent();
	InstancedStaticMeshComponent->SetStaticMesh(SpawnConfiguration->AgentMesh);
	InstancedStaticMeshComponent->SetFlags(RF_Transactional);
	AddInstanceComponent(InstancedStaticMeshComponent);
}

void AAgentsLevelBase::FetchSubsystems()
{
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		SimulationSubsystem = GameInstance->GetSubsystem<USimulationSubsystem>();
	}
}

void AAgentsLevelBase::ScaleBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour, float Scale)
{
	UBaseAutonomousBehaviour* Behaviour = Cast<UBaseAutonomousBehaviour>(TargetBehaviour->GetDefaultObject());
	if(Behaviour != nullptr)
	{
		Behaviour->ScaleInfluence(Scale);
	}
}

void AAgentsLevelBase::ResetBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour)
{
	UBaseAutonomousBehaviour* Behaviour = Cast<UBaseAutonomousBehaviour>(TargetBehaviour->GetDefaultObject());
	if(Behaviour != nullptr)
	{
		Behaviour->ResetInfluence();	
	}
}
