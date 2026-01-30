#include <scheduler.h>
#include <process.h>

static Scheduler system_scheduler;
static uint32_t system_time = 0;
static uint32_t current_quantum = 0;

static void enqueue_process(PCB* process) {
    if (!process) return;
    
    process->next = NULL;
    
    if (!system_scheduler.ready_queue) {
        system_scheduler.ready_queue = process;
    } else {
        PCB* current = system_scheduler.ready_queue;
        while (current->next) {
            current = current->next;
        }
        current->next = process;
    }
}

static PCB* dequeue_process(void) {
    if (!system_scheduler.ready_queue) {
        return NULL;
    }
    
    PCB* process = system_scheduler.ready_queue;
    system_scheduler.ready_queue = system_scheduler.ready_queue->next;
    process->next = NULL;
    return process;
}

void init_scheduler(SchedulerType type, uint32_t quantum) {
    system_scheduler.type = type;
    system_scheduler.ready_queue = NULL;
    system_scheduler.running_process = NULL;
    system_scheduler.quantum = quantum;
    system_scheduler.time_slice = 0;
    system_time = 0;
    current_quantum = 0;
}

void add_process(PCB* process) {
    if (!process) return;
    process->state = STATE_READY;
    process->arrival_time = system_time;
    enqueue_process(process);
}

PCB* schedule(void) {
    if (!system_scheduler.ready_queue) {
        system_scheduler.running_process = NULL;
        return NULL;
    }
    
    if (system_scheduler.type == FCFS) {
        PCB* process = dequeue_process();
        if (process) {
            process->state = STATE_RUNNING;
            system_scheduler.running_process = process;
        }
        return process;
    }
    
    if (system_scheduler.type == ROUND_ROBIN) {
        if (!system_scheduler.running_process || 
            current_quantum >= system_scheduler.quantum ||
            system_scheduler.running_process->state == STATE_TERMINATED) {
            
            if (system_scheduler.running_process && 
                system_scheduler.running_process->state != STATE_TERMINATED) {
                system_scheduler.running_process->state = STATE_READY;
                enqueue_process(system_scheduler.running_process);
            }
            
            PCB* process = dequeue_process();
            if (process) {
                process->state = STATE_RUNNING;
                system_scheduler.running_process = process;
                current_quantum = 0;
            }
            return process;
        }
    }
    
    return system_scheduler.running_process;
}

void run_scheduler_step(void) {
    system_time++;
    
    if (system_scheduler.running_process) {
        execute_process_step(system_scheduler.running_process, 1);
        current_quantum++;
        
        if (is_process_finished(system_scheduler.running_process)) {
            system_scheduler.running_process->state = STATE_TERMINATED;
        }
    }
    
    schedule();
}

uint32_t get_system_time(void) {
    return system_time;
}

uint32_t get_running_pid(void) {
    return system_scheduler.running_process ? system_scheduler.running_process->pid : 0;
}

const char* get_running_process_name(void) {
    return system_scheduler.running_process ? system_scheduler.running_process->name : "IDLE";
}

void run_scheduler_demo(void) {
    // Fonction de démonstration
}