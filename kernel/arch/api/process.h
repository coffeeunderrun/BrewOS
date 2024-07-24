#pragma once

#include <types.h>

typedef enum ProcessState {
    PROCESS_DESTROYED,
    PROCESS_READY,
    PROCESS_ACTIVE,
} ProcessState;

int64_t create_idle_process(char* name);

int64_t create_kernel_process(char* name, void(*entry)());

int64_t create_user_process(int64_t parent_id, char* name, void(*entry)());

void destroy_process(int64_t id);

void sleep_process(uint64_t duration);
