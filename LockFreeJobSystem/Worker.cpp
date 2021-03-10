#include "Worker.h"
#include "Job.h"
#include "Engine.h"

Worker::Worker(Engine* engine, std::size_t InMaxJobs, Mode InMode /*= Mode::Background*/)
	: WorkPool(InMaxJobs)
	, queue(InMaxJobs)
{

}

void Worker::Start()
{

}

void Worker::Stop()
{

}

bool Worker::IsRunning() const
{
	return false;
}

Pool& Worker::GetPool()
{
	return WorkPool;
}

void Worker::Submit(Job* InJob)
{
	queue.Push(InJob);
}

void Worker::Wait(Job* InJob)
{
	while (!InJob->IsFinished())
	{
		Job* job = GetJob();

		if (job)
		{
			job->Run();
		}
	}
}

std::thread::id Worker::GetThreadId() const
{
	return ThreadId;
}

Job* Worker::GetJob()
{
	Job* job = queue.Pop();

	if (job)
	{
		job->Run();
	}
	else
	{
		Worker* worker = engine->GetRandomWorker();

		if (worker != this)
		{
			Job* stolenJob = worker->queue.Steal();

			if (stolenJob)
			{
				return stolenJob;
			}
			else
			{
				std::this_thread::yield();
				return nullptr;
			}
		}
		else
		{
			std::this_thread::yield();
			return nullptr;
		}
	}

	return nullptr;// ?????
}

