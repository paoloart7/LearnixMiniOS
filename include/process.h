#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_TERMINATED
} ProcessState;

typedef struct PCB {
    uint32_t pid;
    char name[32];
    ProcessState state;
    uint32_t priority;
    uint32_t execution_time;
    uint32_t total_time;
    uint32_t arrival_time;
    uint32_t remaining_time;
    uint32_t wait_time;
    struct PCB* next;
} PCB;

PCB* create_process(const char* name, uint32_t priority, uint32_t total_time);
void destroy_process(PCB* process);
void set_process_state(PCB* process, ProcessState state);
const char* state_to_string(ProcessState state);
void execute_process_step(PCB* process, uint32_t time_quantum);
int is_process_finished(const PCB* process);
PCB** create_test_processes(uint32_t* count);

#endif