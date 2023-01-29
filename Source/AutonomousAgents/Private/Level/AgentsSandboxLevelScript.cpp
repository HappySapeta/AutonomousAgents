
#include "Level/AgentsSandboxLevelScript.h"
#include "Configuration/SpawnConfiguration.h"
#include "Subsystems/SpatialGridSubsystem.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/SimulationSubsystem.h"

AAgentsSandboxLevelScript::AAgentsSandboxLevelScript()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAgentsSandboxLevelScript::UpdateInstancedMeshes() const
{
	FTransform NewTransform;
	
	int AgentIndex = 0;
	for(;AgentIndex < NumAgents - 1; ++AgentIndex)
	{
		NewTransform = SimulatorSubsystem->GetTransform(AgentIndex, SpawnConfiguration->RotationOffset);
		InstancedStaticMeshComponent->UpdateInstanceTransform(AgentIndex, NewTransform, true);
	}

	NewTransform = SimulatorSubsystem->GetTransform(AgentIndex, SpawnConfiguration->RotationOffset);
	InstancedStaticMeshComponent->UpdateInstanceTransform(AgentIndex, NewTransform, true, true);
}

void AAgentsSandboxLevelScript::Init(const USpawnConfiguration* Configuration)
{
	checkf(Configuration, TEXT("Configuration cannot be null"));
	SpawnConfiguration = Configuration;
	
	FetchSubsystems();
    CreateInstancedStaticMeshComponent();

	check(SpatialGridSubsystem);
	check(SimulatorSubsystem);
	
	bInitialized = true;
}

void AAgentsSandboxLevelScript::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SpatialGridSubsystem->Update();
	SimulatorSubsystem->Tick(DeltaSeconds);
	UpdateInstancedMeshes();
}

void AAgentsSandboxLevelScript::SpawnSingleAgent(FVector SpawnLocation) const
{
	SpatialGridSubsystem->RegisterAgent(SimulatorSubsystem->CreateAgent(SpawnLocation));
	
	const FTransform& Transform = FTransform(SpawnConfiguration->RotationOffset, SpawnLocation);
	InstancedStaticMeshComponent->AddInstance(Transform);
}

void AAgentsSandboxLevelScript::SpawnAgents()
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
			SpawnSingleAgent(SpawnLocation);
			++NumAgents;
			SpawnLocation.Y += SpawnConfiguration->Separation;
		}
		
		SpawnLocation.X += SpawnConfiguration->Separation;
		SpawnLocation.Y = SpawnConfiguration->Origin.Y - Span;
	}

	const FString Message = FString::Printf(TEXT("[AgentsSandboxLevelScript][SpawnActorsImmediately] %d Actors were spawned."), NumAgents);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
}

void AAgentsSandboxLevelScript::CreateInstancedStaticMeshComponent()
{
	InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
	InstancedStaticMeshComponent->RegisterComponent();
	InstancedStaticMeshComponent->SetStaticMesh(SpawnConfiguration->AgentMesh);
	InstancedStaticMeshComponent->SetFlags(RF_Transactional);
	AddInstanceComponent(InstancedStaticMeshComponent);
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
