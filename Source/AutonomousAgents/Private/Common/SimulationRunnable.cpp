#include "Common/SimulationRunnable.h"

FSimulationRunnable::FSimulationRunnable(const FRunData& Data)
	:RunData(Data)
{
	Thread = FRunnableThread::Create(this, *Data.Name);
}

FSimulationRunnable::~FSimulationRunnable()
{
	if(Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

void FSimulationRunnable::Exit()
{
	FRunnable::Exit();
}

bool FSimulationRunnable::Init()
{
	bStopRequested = false;
	return FRunnable::Init();
}

uint32 FSimulationRunnable::Run()
{
	while(!bStopRequested)
	{
		for(int Index = RunData.LowerLimit; Index <= RunData.UpperLimit; ++Index)
		{
			RunData.SenseNearbyAgents.ExecuteIfBound(Index);
			RunData.ApplyBehaviourOnAgent.ExecuteIfBound(Index);
		}
	}
	
	return 0;
}

void FSimulationRunnable::Stop()
{
	bStopRequested = true;
}

void FSimulationRunnable::Pause()
{
}