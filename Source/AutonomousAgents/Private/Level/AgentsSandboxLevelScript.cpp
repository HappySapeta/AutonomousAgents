
#include "Level/AgentsSandboxLevelScript.h"

#include "Core/AgentPawn.h"
#include "Common/AgentSpawnerConfig.h"
#include "Subsystems/SpatialGridSubsystem.h"
#include "Behaviours/Base/BaseAutonomousBehaviour.h"

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
}

void AAgentsSandboxLevelScript::SpawnActorsImmediately(const UAgentSpawnerConfig* SpawnConfig)
{
	if(SpawnConfig == nullptr) return;
	if(SpawnConfig->AgentClass == nullptr) return;

	if(!SpatialGridSubsystem)
	{
		FetchGridSubsystem();
	}
	
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
			SpawnActor(SpawnConfig, SpawnLocation, SpawnParameters);
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

void AAgentsSandboxLevelScript::SpawnActor(const UAgentSpawnerConfig* SpawnConfig, FVector SpawnLocation, FActorSpawnParameters SpawnParameters) const
{
	AAgentPawn* NewAgent = Cast<AAgentPawn>(GetWorld()->SpawnActor(SpawnConfig->AgentClass, &SpawnLocation, &FRotator::ZeroRotator, SpawnParameters));

	if(SpatialGridSubsystem)
	{
		SpatialGridSubsystem->RegisterActor(NewAgent);
	}
}

void AAgentsSandboxLevelScript::FetchGridSubsystem()
{
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		SpatialGridSubsystem = GameInstance->GetSubsystem<USpatialGridSubsystem>();	
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

