#include <definitions.h>
#include <process.h>

#define MAX_PROCESS_COUNT 256

#define STACK_TOP_SS     -1
#define STACK_TOP_RSP    -2
#define STACK_TOP_RFLAGS -3
#define STACK_TOP_CS     -4
#define STACK_TOP_RIP    -5

typedef struct Process {
    uint64_t* rsp0;
    uint64_t* rsp;
    uint64_t* cr3;

    uint64_t quantum;
    uint64_t sleep;

    int64_t id;
    int64_t parent_id;

    int8_t priority;

    char name[32];

    ProcessState state;
} Process;

extern const void* stack_top;

extern void yield();

static Process processes[MAX_PROCESS_COUNT];
static uint64_t current_process_id;

static Process* get_process_by_id(int64_t);

BREW_EXPORT BREW_API
Process* get_current_process() {
    return get_process_by_id(current_process_id);
}

BREW_EXPORT BREW_API
Process* get_next_process() {
    // Try finding the next available process following current process
    for (int64_t i = current_process_id + 1; i < MAX_PROCESS_COUNT; i++) {
        Process* process = &processes[i];

        if (process->sleep > 0) {
            process->sleep--;
            continue;
        }

        if (process->state == PROCESS_READY) {
            return process;
        }
    }

    // If nothing found, then try from beginning, excluding idle process
    for (int64_t i = 1; i < current_process_id; i++) {
        Process* process = &processes[i];

        if (process->sleep > 0) {
            process->sleep--;
            continue;
        }

        if (process->state == PROCESS_READY) {
            return process;
        }
    }

    // If nothing found, then idle
    return &processes[0];
}

BREW_EXPORT BREW_API
void swap_process(Process* process) {
    if (process == NULL) {
        return;
    }

    Process* current_process = get_current_process();

    if (current_process->state == PROCESS_ACTIVE) {
        current_process->state = PROCESS_READY;
    }

    current_process_id = process->id;
    process->state = PROCESS_ACTIVE;
}

int64_t create_idle_process(char* name) {
    Process* process = &processes[0];

    if (process->state != PROCESS_DESTROYED) {
        return 0;
    }

    process->id = 0;
    process->parent_id = -1;
    process->priority = 0;
    process->state = PROCESS_ACTIVE;
    process->rsp0 = (uint64_t*)&stack_top;
    process->rsp = &process->rsp0[-22];

    return 0;
}

int64_t create_kernel_process(char* name, void(*entry)()) {
    for (int64_t i = 1; i < MAX_PROCESS_COUNT; i++) {
        Process* process = &processes[i];

        if (process->state != PROCESS_DESTROYED) {
            continue;
        }

        process->id = i;
        process->parent_id = -1;
        process->priority = 0;
        process->state = PROCESS_READY;

        process->rsp0 = (uint64_t*)&stack_top - 0x200 * i;
        process->rsp0[STACK_TOP_CS] = 0x08;
        process->rsp0[STACK_TOP_SS] = 0x10;
        process->rsp0[STACK_TOP_RIP] = (uint64_t)entry;
        process->rsp0[STACK_TOP_RSP] = (uint64_t)&stack_top - 0x200 * i;
        process->rsp0[STACK_TOP_RFLAGS] = 0x200;
        process->rsp = &process->rsp0[-22];

        return process->id;
    }

    return -1;
}

int64_t create_user_process(int64_t parent_id, char* name, void(*entry)()) {
    for (int64_t i = 1; i < MAX_PROCESS_COUNT; i++) {
        Process* process = &processes[i];

        if (process->state != PROCESS_DESTROYED) {
            continue;
        }

        Process* parent_process = get_process_by_id(parent_id);

        process->id = i;
        process->parent_id = -1;
        process->priority = 0;
        process->state = PROCESS_READY;

        process->rsp0 = (uint64_t*)&stack_top - 0x200 * i;
        process->rsp0[STACK_TOP_CS] = 0x08;
        process->rsp0[STACK_TOP_SS] = 0x10;
        process->rsp0[STACK_TOP_RIP] = (uint64_t)entry;
        process->rsp0[STACK_TOP_RSP] = (uint64_t)&stack_top - 0x200 * i;
        process->rsp = &process->rsp0[-22];

        if (parent_process == NULL) {
            return process->id;
        }

        process->parent_id = parent_process->id;
        process->priority = parent_process->priority;
        process->cr3 = parent_process->cr3;
        process->rsp0[STACK_TOP_RFLAGS] = parent_process->rsp0[STACK_TOP_RFLAGS];

        return process->id;
    }

    return -1;
}

void destroy_process(int64_t id) {
    Process* process = get_process_by_id(id);

    if (process == NULL) {
        return;
    }

    process->state = PROCESS_DESTROYED;
}

void sleep_process(uint64_t duration) {
    Process* process = get_current_process();

    if (process == NULL) {
        return;
    }

    process->sleep = duration;

    yield();
}

static Process* get_process_by_id(int64_t id) {
    if (id < 0 || id >= MAX_PROCESS_COUNT) {
        return NULL;
    }

    return &processes[id];
}
