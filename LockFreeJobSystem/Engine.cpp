#include "Engine.h"
#include <atomic>
#include <random>

Engine::Engine(std::size_t InNumThreads, std::size_t InJobsPerThread)
	: Workers(InNumThreads)
{
	std::size_t jobsPerQueue = InJobsPerThread;
	Workers.EmplaceBack(this, jobsPerQueue, Worker::Mode::Foreground);

	for (std::size_t i = 1; i < InNumThreads; ++i)
	{
		Workers.EmplaceBack(this, jobsPerQueue, Worker::Mode::Background);
	}

	for (std::size_t i = 0; i < Workers.CurrentSize(); ++i)
	{
		Workers[i].Start();
	}
}

Worker* Engine::GetRandomWorker()
{
	std::uniform_int_distribution<std::size_t> dist{ 0, Workers.CurrentSize()-1 };
	std::default_random_engine randomEngine{ std::random_device()() };

	Worker* worker = &Workers[dist(randomEngine)];

	if (worker->IsRunning())
	{
		return worker;
	}

	return nullptr;
}

Worker* Engine::GetThreadWorker()
{
	return FindThreadWorker(std::this_thread::get_id());
}

Worker* Engine::FindThreadWorker(const std::thread::id InThreadId)
{
	for (std::size_t i = 0; i < Workers.CurrentSize(); ++i)
	{
		if (Workers[i].GetThreadId() == InThreadId)
		{
			return &Workers[i];
		}
	}
	return nullptr;
}

