#include "JobQueue.h"

JobQueue::JobQueue(std::size_t InMaxJobs)
	: Jobs{InMaxJobs}
{

}

bool JobQueue::Push(Job* InJob)
{
	int bottom = Bottom.load(std::memory_order_acquire);

	if (bottom < static_cast<int>(Jobs.size()))
	{
		Jobs[bottom] = InJob;
		Bottom.store(bottom + 1, std::memory_order_release);

		return true;
	}
	else
	{
		return false;
	}
}

Job* JobQueue::Pop()
{
	int bottom = Bottom.load(std::memory_order_relaxed);
	bottom = std::max(0, bottom - 1);
	Bottom.store(bottom, std::memory_order_seq_cst);

	int top = Top.load(std::memory_order_seq_cst);

	if (top <= bottom)
	{
		Job* job = Jobs[bottom];
		if (top != bottom)
		{
			return job;
		}
		else
		{
			int expectedTop = top;
			int desiredTop = top + 1;

			if (!Top.compare_exchange_strong(expectedTop, desiredTop, std::memory_order_relaxed))
			{
				// Job was stolen already
				job = nullptr;
			}

			Bottom.store(top + 1, std::memory_order_relaxed);
			return job;
		}
	}
	else
	{
		Bottom.store(top, std::memory_order_relaxed);
		return nullptr;
	}
}

Job* JobQueue::Steal()
{
	int top = Top.load(std::memory_order_relaxed);
	int bottom = Bottom.load(std::memory_order_seq_cst);
	if (top < bottom)
	{
		Job* job = Jobs[top];

		if (Top.compare_exchange_weak(top, top + 1, std::memory_order_seq_cst) == false)
		{
			return nullptr;
		}

		return job;
	}
	else
	{
		return nullptr;
	}
}
