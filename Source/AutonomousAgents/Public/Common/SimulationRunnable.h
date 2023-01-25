#pragma once

#include "CoreMinimal.h"
#include "SimulationRunnable.generated.h"

class UAgentData;
class USimulationSubsystem;

USTRUCT()
struct FRunData
{
	GENERATED_BODY();

	DECLARE_DELEGATE_OneParam(FRunnableCallback, uint32)

	FRunData() = default;
	
	FRunData(const int LowerLimit, const int UpperLimit,
	         const FRunnableCallback& Function_SenseNearbyAgents,
	         const FRunnableCallback& Function_ApplyBehaviourOnAgent,
	         const FRunnableCallback& Function_UpdateAgentState,
	         const FString ThreadName)
	:
	LowerLimit(LowerLimit),
	UpperLimit(UpperLimit),
	SenseNearbyAgents(Function_SenseNearbyAgents),
	ApplyBehaviourOnAgent(Function_ApplyBehaviourOnAgent),
	UpdateAgentState(Function_UpdateAgentState),
	Name(ThreadName)
	{}

	int LowerLimit = -1;
	int UpperLimit = -1;

	FRunnableCallback SenseNearbyAgents;
	FRunnableCallback ApplyBehaviourOnAgent;
	FRunnableCallback UpdateAgentState;

	FString Name;
};

class FSimulationRunnable : public FRunnable
{
public:
	FSimulationRunnable(const FRunData& Data);

	virtual ~FSimulationRunnable();

public:
	virtual void Exit() override;

	virtual bool Init() override;

	virtual uint32 Run() override;

	virtual void Stop() override;

	virtual void Pause();

private:
	FRunData RunData;
	FRunnableThread* Thread = nullptr;
	bool bStopRequested = false;
};
