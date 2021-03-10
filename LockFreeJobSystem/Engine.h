#pragma once
#include "Worker.h"
#include "StaticVector.h"

class Engine
{
public:
	Engine(std::size_t InNumThreads, std::size_t InJobsPerThread);

	Worker* GetRandomWorker();
	Worker* GetThreadWorker();

private:
	StaticVector<Worker> Workers;

	Worker* FindThreadWorker(const std::thread::id InThreadId);
};