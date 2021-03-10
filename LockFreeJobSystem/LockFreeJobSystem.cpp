// LockFreeJobSystem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Engine.h"
#include "Worker.h"
#include "Job.h"
#include "Pool.h"

int main()
{
    std::size_t kMaxTestJobs = 60;

    Engine engine{4, kMaxTestJobs };
    Worker* worker = engine.GetThreadWorker();
    
    Job* root = worker->GetPool().CreateJob([](Job& job) {
    
    });

    for (std::size_t i = 0; i < kMaxTestJobs; ++i)
    {
        Job* child = worker->GetPool().CreateClosureJobAsChild([i](Job& job) {
            std::cout << std::to_string(i) << std::endl;
		}, root);
		worker->Submit(child);
    }

    worker->Submit(root);
    worker->Wait(root);

    return 0;
}