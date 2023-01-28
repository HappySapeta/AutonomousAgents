
#include "Level/AgentsSandboxLevelScript.h"
#include "Common/AgentSpawnerConfig.h"
#include "Subsystems/SpatialGridSubsystem.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/SimulationSubsystem.h"

AAgentsSandboxLevelScript::AAgentsSandboxLevelScript()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAgentsSandboxLevelScript::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(SpatialGridSubsystem)
	{
		SpatialGridSubsystem->Update();
	}
	if(SimulatorSubsystem)
	{
		SimulatorSubsystem->Update(DeltaSeconds);
	}

	int AgentIndex = 0;
	FTransform NewTransform = FTransform::Identity;
	for(;AgentIndex < SimulatorSubsystem->GetNumAgents() - 1; ++AgentIndex)
	{
		InstancedStaticMeshComponent->UpdateInstanceTransform(AgentIndex, SimulatorSubsystem->GetTransform(AgentIndex, OffsetRotation), true);
	}

	InstancedStaticMeshComponent->UpdateInstanceTransform(AgentIndex, SimulatorSubsystem->GetTransform(AgentIndex, OffsetRotation), true, true);
}

void AAgentsSandboxLevelScript::SpawnActorsImmediately(const UAgentSpawnerConfig* SpawnConfig)
{
	if(SpawnConfig == nullptr)
	{
		return;
	}
	
	FetchSubsystems();
	
	InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
	InstancedStaticMeshComponent->RegisterComponent();
	InstancedStaticMeshComponent->SetStaticMesh(AgentMesh);
	InstancedStaticMeshComponent->SetFlags(RF_Transactional);
	AddInstanceComponent(InstancedStaticMeshComponent);
	
	FVector SpawnLocation = SpawnConfig->Origin - FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);

	const FVector& LowerBound = SpawnConfig->Origin - FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);
	const FVector& UpperBound = SpawnConfig->Origin + FVector(SpawnConfig->Span, SpawnConfig->Span, 0.0f);

	FActorSpawnParameters SpawnParameters;
#if UE_EDITOR
	SpawnParameters.bHideFromSceneOutliner = SpawnConfig->bHideFromSceneOutliner;
#endif
	SpawnParameters.ObjectFlags |= RF_Transient;

	uint32 NumAgentsSpawned = 0;
	while(SpawnLocation.X >= LowerBound.X && SpawnLocation.X <= UpperBound.X)
	{
		while(SpawnLocation.Y >= LowerBound.Y && SpawnLocation.Y <= UpperBound.Y)
		{
			SpawnAgent(SpawnLocation);
			++NumAgentsSpawned;
			SpawnLocation.Y += SpawnConfig->Separation;
		}
		
		SpawnLocation.X += SpawnConfig->Separation;
		SpawnLocation.Y = SpawnConfig->Origin.Y - SpawnConfig->Span;
	}

	const FString Message = FString::Printf(TEXT("[AgentsSandboxLevelScript][SpawnActorsImmediately] %d Actors were spawned."), NumAgentsSpawned);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
}

void AAgentsSandboxLevelScript::SpawnAgent(FVector SpawnLocation) const
{
	SpatialGridSubsystem->RegisterAgent(SimulatorSubsystem->CreateAgent(SpawnLocation));
	
	const FTransform& Transform = FTransform(OffsetRotation, SpawnLocation);
	InstancedStaticMeshComponent->AddInstance(Transform);
}

void AAgentsSandboxLevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void AAgentsSandboxLevelScript::FetchSubsystems()
{
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		SpatialGridSubsystem = GameInstance->GetSubsystem<USpatialGridSubsystem>();
		SimulatorSubsystem = GameInstance->GetSubsystem<USimulationSubsystem>();
	}
}

void AAgentsSandboxLevelScript::ScaleBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour, float Scale)
{
	UBaseAutonomousBehaviour* Behaviour = Cast<UBaseAutonomousBehaviour>(TargetBehaviour->GetDefaultObject());
	if(Behaviour != nullptr)
	{
		Behaviour->ScaleInfluence(Scale);	
	}
}

void AAgentsSandboxLevelScript::ResetBehaviourInfluence(TSubclassOf<UBaseAutonomousBehaviour> TargetBehaviour)
{
	UBaseAutonomousBehaviour* Behaviour = Cast<UBaseAutonomousBehaviour>(TargetBehaviour->GetDefaultObject());
	if(Behaviour != nullptr)
	{
		Behaviour->ResetInfluence();	
	}
}

void AAgentsSandboxLevelScript::StartSimulation()
{
	if(SimulatorSubsystem)
	{
		SimulatorSubsystem->StartSimulation();
	}
}
