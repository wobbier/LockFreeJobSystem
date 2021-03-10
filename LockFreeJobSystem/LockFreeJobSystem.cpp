// LockFreeJobSystem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Engine.h"
#include "Worker.h"
#include "Job.h"
#include "Pool.h"
#include <chrono>

int main()
{
	std::size_t kMaxTestJobs = 60000;
	std::size_t kMaxWorkers = 12;

    std::chrono::high_resolution_clock clock;
    auto startTime = clock.now();

    Engine engine{ kMaxWorkers, kMaxTestJobs };
    Worker* worker = engine.GetThreadWorker();
    
    Job* root = worker->GetPool().CreateJob([](Job& job) {
    
    });

    for (std::size_t i = 0; i < kMaxTestJobs-1; ++i)
    {
        Job* child = worker->GetPool().CreateClosureJobAsChild([i](Job& job) {
            std::cout << std::to_string(i) << std::endl;
		}, root);
		worker->Submit(child);
    }

    worker->Submit(root);
    worker->Wait(root);

    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - startTime);
	std::cout << "Completed " << std::to_string(kMaxTestJobs) << " Jobs in " << std::chrono::milliseconds(endTime).count() << " milliseconds using " << std::to_string(kMaxWorkers) << " worker threads." << std::endl;
    std::cin.get();

    return 0;
}