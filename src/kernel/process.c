#include <process.h>

static uint32_t next_pid = 1;

PCB* create_process(const char* name, uint32_t priority, uint32_t total_time) {
    static PCB process;
    
    process.pid = next_pid++;
    
    int i = 0;
    while (i < 31 && name[i] != '\0') {
        process.name[i] = name[i];
        i++;
    }
    process.name[i] = '\0';
    
    process.state = STATE_READY;
    process.priority = priority;
    process.total_time = total_time;
    process.execution_time = 0;
    process.arrival_time = 0;
    process.remaining_time = total_time;
    process.wait_time = 0;
    process.next = NULL;
    
    return &process;
}

// ... autres fonctions simples ...