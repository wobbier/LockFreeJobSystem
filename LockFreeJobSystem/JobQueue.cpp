#include "JobQueue.h"

JobQueue::JobQueue(std::size_t InMaxJobs)
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
	int bottom = Bottom.load(std::memory_order_acquire);
	bottom = std::max(0, bottom - 1);
	Bottom.store(bottom, std::memory_order_release);

	int top = Top.load(std::memory_order_acquire);

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

			if (!Top.compare_exchange_weak(expectedTop, desiredTop, std::memory_order_acq_rel))
			{
				// Job was stolen already
				job = nullptr;
			}

			//Top?
			Bottom.store(top + 1, std::memory_order_release);
			return job;
		}
	}
	else
	{
		//Top?
		Bottom.store(top, std::memory_order_release);
		return nullptr;
	}
}

Job* JobQueue::Steal()
{
	return nullptr;
}

