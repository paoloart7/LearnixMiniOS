#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "process.h"

typedef enum {
    FCFS,
    ROUND_ROBIN,
    PRIORITY,
    SJF,
    SRTF,
    MLFQ
} SchedulerType;

typedef struct {
    SchedulerType type;
    PCB* ready_queue;
    PCB* running_process;
    uint32_t quantum;
    uint32_t time_slice;
} Scheduler;

void init_scheduler(SchedulerType type, uint32_t quantum);
void add_process(PCB* process);
PCB* schedule(void);
void run_scheduler_step(void);
uint32_t get_system_time(void);
uint32_t get_running_pid(void);
const char* get_running_process_name(void);
void run_scheduler_demo(void);

#endif