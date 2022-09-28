#ifndef BREWOS_SCHEDULER_H
#define BREWOS_SCHEDULER_H

namespace BrewOS::Scheduler
{
    typedef void (*Entry)();
    
    void Start();

    void Yield();

    void AddProcess(Entry entry);
}

#endif // BREWOS_SCHEDULER_H
