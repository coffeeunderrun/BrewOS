#ifndef BREWOS_SCHEDULER_H
#define BREWOS_SCHEDULER_H

namespace BrewOS::Scheduler
{
    void Start();

    void Switch();

    void AddProcess(Process *process);
}

#endif // BREWOS_SCHEDULER_H
