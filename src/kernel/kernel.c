// =============================================================================
// LearnixMiniOS v1.0 - Projet INF412
// =============================================================================

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;

#define NULL ((void*)0)

// =============================================================================
// I/O PORTS
// =============================================================================
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void cpu_pause(void) {
    asm volatile ("pause");
}

// =============================================================================
// VGA TEXT MODE (80x25 fixe)
// =============================================================================
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static volatile uint16_t* const VGA = (volatile uint16_t*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;

#define COLOR_BLACK   0x0
#define COLOR_BLUE    0x1
#define COLOR_GREEN   0x2
#define COLOR_CYAN    0x3
#define COLOR_RED     0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN   0x6
#define COLOR_LGRAY   0x7
#define COLOR_DGRAY   0x8
#define COLOR_LBLUE   0x9
#define COLOR_LGREEN  0xA
#define COLOR_LCYAN   0xB
#define COLOR_LRED    0xC
#define COLOR_LMAGENTA 0xD
#define COLOR_YELLOW  0xE
#define COLOR_WHITE   0xF

static inline uint8_t vga_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void update_cursor(void) {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void enable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void vga_clear(void) {
    uint8_t color = vga_color(COLOR_LGRAY, COLOR_BLACK);
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = vga_entry(' ', color);
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void vga_scroll(void) {
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[y * VGA_WIDTH + x] = VGA[(y + 1) * VGA_WIDTH + x];
        }
    }
    uint8_t color = vga_color(COLOR_LGRAY, COLOR_BLACK);
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', color);
    }
}

void vga_putchar_color(char c, uint8_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA[cursor_y * VGA_WIDTH + cursor_x] = vga_entry(' ', color);
        }
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~3;
    } else {
        VGA[cursor_y * VGA_WIDTH + cursor_x] = vga_entry(c, color);
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    while (cursor_y >= VGA_HEIGHT) {
        vga_scroll();
        cursor_y--;
    }
    
    update_cursor();
}

void vga_putchar(char c) {
    vga_putchar_color(c, vga_color(COLOR_LGRAY, COLOR_BLACK));
}

void vga_print_color(const char* str, uint8_t color) {
    while (*str) {
        vga_putchar_color(*str++, color);
    }
}

void vga_print(const char* str) {
    vga_print_color(str, vga_color(COLOR_LGRAY, COLOR_BLACK));
}

void vga_print_int(int num, uint8_t color) {
    char buf[12];
    int i = 10;
    int neg = 0;
    
    if (num < 0) { neg = 1; num = -num; }
    buf[11] = '\0';
    
    if (num == 0) {
        buf[i--] = '0';
    } else {
        while (num > 0 && i >= 0) {
            buf[i--] = '0' + (num % 10);
            num /= 10;
        }
    }
    if (neg) buf[i--] = '-';
    
    vga_print_color(&buf[i + 1], color);
}

// =============================================================================
// STRING FUNCTIONS
// =============================================================================
int k_strlen(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int k_strncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void k_strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void k_strcat(char* dest, const char* src) {
    while (*dest) dest++;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void k_itoa(int num, char* buf) {
    int i = 0;
    int neg = 0;
    
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    
    if (num < 0) {
        neg = 1;
        num = -num;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (neg) buf[i++] = '-';
    buf[i] = '\0';
    
    // Reverse
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
}

// =============================================================================
// KEYBOARD (avec optimisation CPU)
// =============================================================================
static const char scancode_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

char keyboard_getchar(void) {
    uint8_t scancode;
    while (1) {
        // Optimisation CPU : pause au lieu de boucle active
        while (!(inb(0x64) & 0x01)) {
            cpu_pause();
        }
        scancode = inb(0x60);
        if (scancode & 0x80) continue;
        if (scancode < sizeof(scancode_ascii)) {
            char c = scancode_ascii[scancode];
            if (c != 0) return c;
        }
    }
}

// =============================================================================
// LOG SYSTEM
// =============================================================================
#define MAX_LOG 15
#define LOG_SIZE 45

typedef struct {
    uint32_t time;
    char msg[LOG_SIZE];
} LogEntry;

static LogEntry logs[MAX_LOG];
static int log_count = 0;
static int log_start = 0;

void log_event(const char* msg) {
    extern uint32_t system_time;
    int idx = (log_start + log_count) % MAX_LOG;
    if (log_count >= MAX_LOG) {
        log_start = (log_start + 1) % MAX_LOG;
    } else {
        log_count++;
    }
    logs[idx].time = system_time;
    int i = 0;
    while (msg[i] && i < LOG_SIZE - 1) { 
        logs[idx].msg[i] = msg[i]; 
        i++; 
    }
    logs[idx].msg[i] = '\0';
}

// =============================================================================
// PROCESS MANAGEMENT
// =============================================================================
typedef enum { STATE_READY, STATE_RUNNING, STATE_BLOCKED, STATE_TERMINATED } ProcessState;

typedef struct {
    uint32_t pid;
    char name[12];
    ProcessState state;
    uint32_t burst_time;
    uint32_t remaining_time;
    uint32_t wait_time;
} PCB;

#define MAX_PROC 8
static PCB procs[MAX_PROC];
static int proc_count = 0;
static int cur_proc = -1;
static int last_proc = -1;  // Pour Round Robin circulaire
static uint32_t next_pid = 1;
uint32_t system_time = 0;

#define TOTAL_MEM 1024
static uint32_t used_mem = 128;

const char* state_str(ProcessState s) {
    switch (s) {
        case STATE_READY: return "READY";
        case STATE_RUNNING: return "RUN";
        case STATE_BLOCKED: return "BLOCK";
        case STATE_TERMINATED: return "DONE";
        default: return "?";
    }
}

int create_proc(const char* name, uint32_t burst) {
    if (proc_count >= MAX_PROC) return -1;
    
    PCB* p = &procs[proc_count];
    p->pid = next_pid++;
    
    int i = 0;
    while (name[i] && i < 11) { p->name[i] = name[i]; i++; }
    p->name[i] = '\0';
    
    p->state = STATE_READY;
    p->burst_time = burst;
    p->remaining_time = burst;
    p->wait_time = 0;
    used_mem += 24;
    proc_count++;
    
    // Log détaillé
    char log_msg[LOG_SIZE];
    char num_buf[8];
    k_strcpy(log_msg, "Cree: ");
    k_strcat(log_msg, p->name);
    k_strcat(log_msg, " (PID ");
    k_itoa(p->pid, num_buf);
    k_strcat(log_msg, num_buf);
    k_strcat(log_msg, ", burst=");
    k_itoa(burst, num_buf);
    k_strcat(log_msg, num_buf);
    k_strcat(log_msg, ")");
    log_event(log_msg);
    
    return p->pid;
}

void kill_proc(uint32_t pid) {
    for (int i = 0; i < proc_count; i++) {
        if (procs[i].pid == pid && procs[i].state != STATE_TERMINATED) {
            procs[i].state = STATE_TERMINATED;
            if (cur_proc == i) cur_proc = -1;
            if (used_mem >= 24) used_mem -= 24;
            
            // Log détaillé
            char log_msg[LOG_SIZE];
            char num_buf[8];
            k_strcpy(log_msg, "Termine: ");
            k_strcat(log_msg, procs[i].name);
            k_strcat(log_msg, " (PID ");
            k_itoa(pid, num_buf);
            k_strcat(log_msg, num_buf);
            k_strcat(log_msg, ")");
            log_event(log_msg);
            
            return;
        }
    }
}

// =============================================================================
// SCHEDULER (avec correction Round Robin)
// =============================================================================
typedef enum { SCHED_FCFS, SCHED_RR } SchedType;
static SchedType sched_type = SCHED_FCFS;
static uint32_t quantum = 2;
static uint32_t cur_quantum = 0;
static int preemption_occurred = 0;
static char preempted_name[12] = "";
static char new_proc_name[12] = "";

void sched_step(void) {
    system_time++;
    preemption_occurred = 0;
    preempted_name[0] = '\0';
    new_proc_name[0] = '\0';
    
    // Incrémenter temps d'attente des READY
    for (int i = 0; i < proc_count; i++) {
        if (procs[i].state == STATE_READY) procs[i].wait_time++;
    }
    
    if (cur_proc >= 0 && procs[cur_proc].state == STATE_RUNNING) {
        procs[cur_proc].remaining_time--;
        cur_quantum++;
        
        if (procs[cur_proc].remaining_time == 0) {
            // Processus terminé
            char log_msg[LOG_SIZE];
            k_strcpy(log_msg, "Termine: ");
            k_strcat(log_msg, procs[cur_proc].name);
            log_event(log_msg);
            
            procs[cur_proc].state = STATE_TERMINATED;
            if (used_mem >= 24) used_mem -= 24;
            last_proc = cur_proc;
            cur_proc = -1;
            cur_quantum = 0;
        } 
        else if (sched_type == SCHED_RR && cur_quantum >= quantum) {
            // Préemption Round Robin
            preemption_occurred = 1;
            k_strcpy(preempted_name, procs[cur_proc].name);
            
            char log_msg[LOG_SIZE];
            k_strcpy(log_msg, "Preemption: ");
            k_strcat(log_msg, procs[cur_proc].name);
            log_event(log_msg);
            
            procs[cur_proc].state = STATE_READY;
            last_proc = cur_proc;
            cur_proc = -1;
            cur_quantum = 0;
        }
    }
    
    // Élire nouveau processus (file circulaire pour RR)
    if (cur_proc < 0) {
        int start = (last_proc + 1) % proc_count;
        if (proc_count == 0) start = 0;
        
        for (int i = 0; i < proc_count; i++) {
            int idx = (start + i) % proc_count;
            if (procs[idx].state == STATE_READY) {
                procs[idx].state = STATE_RUNNING;
                cur_proc = idx;
                cur_quantum = 0;
                
                if (preemption_occurred) {
                    k_strcpy(new_proc_name, procs[idx].name);
                }
                
                char log_msg[LOG_SIZE];
                k_strcpy(log_msg, "Execute: ");
                k_strcat(log_msg, procs[idx].name);
                log_event(log_msg);
                
                break;
            }
        }
    }
}

// =============================================================================
// SHELL
// =============================================================================
#define CMD_SIZE 48
static char cmd_buf[CMD_SIZE];
static int cmd_pos = 0;
static int running = 1;

void print_prompt(void) {
    vga_print("\n");
    vga_print_color("learnix", vga_color(COLOR_LGREEN, COLOR_BLACK));
    vga_print_color("> ", vga_color(COLOR_WHITE, COLOR_BLACK));
    update_cursor();
}

void print_help(void) {
    vga_print_color("\n=== Commandes ===\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    vga_print("  help       - Aide\n");
    vga_print("  clear      - Effacer\n");
    vga_print("  about      - A propos\n");
    vga_print("  ps         - Processus\n");
    vga_print("  run X [N]  - Creer processus X (burst=N, defaut 5)\n");
    vga_print("  kill N     - Tuer PID N\n");
    vga_print("  fcfs       - Mode FCFS\n");
    vga_print("  rr         - Mode Round Robin\n");
    vga_print("  demo       - Demo scheduler\n");
    vga_print("  step       - Un pas scheduler\n");
    vga_print("  time       - Temps systeme\n");
    vga_print("  mem        - Memoire\n");
    vga_print("  log        - Journal\n");
    vga_print("  exit       - Quitter\n");
}

void print_ps(void) {
    vga_print_color("\nPID  NOM         ETAT   REST  WAIT\n", vga_color(COLOR_CYAN, COLOR_BLACK));
    vga_print("---  ----------  -----  ----  ----\n");
    
    if (proc_count == 0) {
        vga_print_color("  (aucun processus)\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
        return;
    }
    
    for (int i = 0; i < proc_count; i++) {
        vga_print_int(procs[i].pid, vga_color(COLOR_WHITE, COLOR_BLACK));
        if (procs[i].pid < 10) vga_print("    ");
        else vga_print("   ");
        
        vga_print_color(procs[i].name, vga_color(COLOR_LGREEN, COLOR_BLACK));
        for (int j = k_strlen(procs[i].name); j < 12; j++) vga_putchar(' ');
        
        uint8_t sc = vga_color(COLOR_LGRAY, COLOR_BLACK);
        if (procs[i].state == STATE_RUNNING) sc = vga_color(COLOR_LGREEN, COLOR_BLACK);
        else if (procs[i].state == STATE_READY) sc = vga_color(COLOR_YELLOW, COLOR_BLACK);
        else if (procs[i].state == STATE_TERMINATED) sc = vga_color(COLOR_DGRAY, COLOR_BLACK);
        
        vga_print_color(state_str(procs[i].state), sc);
        for (int j = k_strlen(state_str(procs[i].state)); j < 7; j++) vga_putchar(' ');
        
        vga_print_int(procs[i].remaining_time, vga_color(COLOR_WHITE, COLOR_BLACK));
        if (procs[i].remaining_time < 10) vga_print("     ");
        else vga_print("    ");
        
        vga_print_int(procs[i].wait_time, vga_color(COLOR_LGRAY, COLOR_BLACK));
        vga_print("\n");
    }
    
    vga_print("\nScheduler: ");
    vga_print_color(sched_type == SCHED_FCFS ? "FCFS" : "Round Robin", vga_color(COLOR_CYAN, COLOR_BLACK));
    vga_print("\n");
}

void print_mem(void) {
    vga_print_color("\n=== Memoire ===\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    vga_print("Total: ");
    vga_print_int(TOTAL_MEM, vga_color(COLOR_WHITE, COLOR_BLACK));
    vga_print(" Ko\n");
    vga_print("Utilise: ");
    vga_print_int(used_mem, vga_color(COLOR_LGREEN, COLOR_BLACK));
    vga_print(" Ko\n");
    vga_print("Libre: ");
    vga_print_int(TOTAL_MEM - used_mem, vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print(" Ko\n");
    
    vga_print("[");
    int bars = (used_mem * 30) / TOTAL_MEM;
    for (int i = 0; i < 30; i++) {
        if (i < bars) vga_print_color("#", vga_color(COLOR_LGREEN, COLOR_BLACK));
        else vga_print_color(".", vga_color(COLOR_DGRAY, COLOR_BLACK));
    }
    vga_print("] ");
    vga_print_int((used_mem * 100) / TOTAL_MEM, vga_color(COLOR_WHITE, COLOR_BLACK));
    vga_print("%\n");
}

void print_log(void) {
    vga_print_color("\n=== Journal ===\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    if (log_count == 0) {
        vga_print_color("  (vide)\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
        return;
    }
    for (int i = 0; i < log_count; i++) {
        int idx = (log_start + i) % MAX_LOG;
        vga_print("[");
        vga_print_int(logs[idx].time, vga_color(COLOR_CYAN, COLOR_BLACK));
        vga_print("] ");
        vga_print(logs[idx].msg);
        vga_print("\n");
    }
}

void run_demo(void) {
    vga_print_color("\n=== Demo Scheduler ===\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    
    // Reset
    proc_count = 0;
    cur_proc = -1;
    last_proc = -1;
    system_time = 0;
    next_pid = 1;
    used_mem = 128;
    log_count = 0;
    log_start = 0;
    
    create_proc("P1", 3);
    create_proc("P2", 4);
    create_proc("P3", 2);
    
    vga_print("Mode: ");
    vga_print_color(sched_type == SCHED_FCFS ? "FCFS" : "Round Robin (quantum=2)", vga_color(COLOR_CYAN, COLOR_BLACK));
    vga_print("\n\n");
    
    vga_print_color("T   Processus   Rest   Action\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
    vga_print("--  ----------  ----   ------\n");
    
    for (int t = 0; t < 12; t++) {
        // Afficher temps
        if (t < 10) vga_print(" ");
        vga_print_int(t, vga_color(COLOR_WHITE, COLOR_BLACK));
        vga_print("  ");
        
        // Afficher processus en cours
        if (cur_proc >= 0 && procs[cur_proc].state == STATE_RUNNING) {
            vga_print_color(procs[cur_proc].name, vga_color(COLOR_LGREEN, COLOR_BLACK));
            for (int j = k_strlen(procs[cur_proc].name); j < 12; j++) vga_putchar(' ');
            
            vga_print_int(procs[cur_proc].remaining_time, vga_color(COLOR_YELLOW, COLOR_BLACK));
            if (procs[cur_proc].remaining_time < 10) vga_print("      ");
            else vga_print("     ");
        } else {
            vga_print_color("(idle)      -      ", vga_color(COLOR_DGRAY, COLOR_BLACK));
        }
        
        sched_step();
        
        // Afficher action
        if (preemption_occurred) {
            vga_print_color("PREEMPTION -> ", vga_color(COLOR_LRED, COLOR_BLACK));
            vga_print_color(new_proc_name, vga_color(COLOR_LCYAN, COLOR_BLACK));
        } else if (cur_proc >= 0) {
            vga_print_color("exec", vga_color(COLOR_LGREEN, COLOR_BLACK));
        }
        vga_print("\n");
    }
    
    vga_print("\n");
    vga_print_color("Demo terminee.\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
}

void print_about(void) {
    vga_print_color("\n=== LearnixMiniOS v1.0 ===\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print("\nProjet academique pour le cours:\n");
    vga_print_color(" Noyau des Systemes d'Exploitation (INF412)\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    vga_print_color(" Cree par l'etudiant Karlsen PAUL\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    vga_print_color("\nFonctionnalites:\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
    vga_print("  * Bootloader et noyau 32 bits\n");
    vga_print("  * Gestion de processus (PCB, 4 etats)\n");
    vga_print("  * Ordonnancement FCFS et Round Robin\n");
    vga_print("  * Timer systeme\n");
    vga_print("  * Journal d'execution\n");
    vga_print("  * Shell interactif\n");
    vga_print("  * Gestion memoire basique\n");
    
    vga_print_color("\nTechnologies:\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
    vga_print("  * Langage C + Assembleur x86\n");
    vga_print("  * Cross-compiler i686-elf-gcc\n");
    vga_print("  * Emulateur QEMU\n");
}

void print_shutdown(void) {
    vga_clear();
    vga_print("\n\n\n");
    vga_print_color("        === ARRET DU SYSTEME ===\n\n", vga_color(COLOR_YELLOW, COLOR_BLACK));
    vga_print("    Merci d'avoir utilise LearnixMiniOS!\n\n");
    vga_print("    Projet INF412 - Systemes d'Exploitation\n\n");
    vga_print_color("    Fermez la fenetre QEMU pour quitter.\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
}

// Parser pour extraire nom et burst de "run X N"
void parse_run_cmd(const char* args, char* name, uint32_t* burst) {
    int i = 0;
    int j = 0;
    
    // Extraire le nom
    while (args[i] && args[i] != ' ' && j < 11) {
        name[j++] = args[i++];
    }
    name[j] = '\0';
    
    // Burst par défaut
    *burst = 5;
    
    // Chercher le burst s'il existe
    while (args[i] == ' ') i++;
    
    if (args[i] >= '0' && args[i] <= '9') {
        *burst = 0;
        while (args[i] >= '0' && args[i] <= '9') {
            *burst = (*burst * 10) + (args[i] - '0');
            i++;
        }
        if (*burst == 0) *burst = 1;
        if (*burst > 99) *burst = 99;
    }
}

void exec_cmd(const char* cmd) {
    if (k_strcmp(cmd, "help") == 0) {
        print_help();
    }
    else if (k_strcmp(cmd, "clear") == 0) {
        vga_clear();
    }
    else if (k_strcmp(cmd, "ps") == 0) {
        print_ps();
    }
    else if (k_strcmp(cmd, "mem") == 0) {
        print_mem();
    }
    else if (k_strcmp(cmd, "log") == 0) {
        print_log();
    }
    else if (k_strcmp(cmd, "about") == 0) {
        print_about();
    }
    else if (k_strcmp(cmd, "demo") == 0) {
        run_demo();
    }
    else if (k_strcmp(cmd, "fcfs") == 0) {
        sched_type = SCHED_FCFS;
        log_event("Scheduler: FCFS");
        vga_print_color("Scheduler: FCFS\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
        vga_print_color("  Premier arrive, premier servi.\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
        vga_print_color("  Pas de preemption.\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
    }
    else if (k_strcmp(cmd, "rr") == 0) {
        sched_type = SCHED_RR;
        log_event("Scheduler: Round Robin");
        vga_print_color("Scheduler: Round Robin\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
        vga_print_color("  Quantum = 2 ticks.\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
        vga_print_color("  Preemption apres chaque quantum.\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
    }
    else if (k_strcmp(cmd, "step") == 0) {
        sched_step();
        vga_print("T=");
        vga_print_int(system_time, vga_color(COLOR_CYAN, COLOR_BLACK));
        
        if (preemption_occurred) {
            vga_print_color(" PREEMPTION: ", vga_color(COLOR_LRED, COLOR_BLACK));
            vga_print(preempted_name);
            vga_print_color(" -> ", vga_color(COLOR_WHITE, COLOR_BLACK));
            vga_print_color(new_proc_name, vga_color(COLOR_LCYAN, COLOR_BLACK));
            vga_print_color(" (quantum atteint)", vga_color(COLOR_DGRAY, COLOR_BLACK));
        } else if (cur_proc >= 0) {
            vga_print(" | ");
            vga_print_color(procs[cur_proc].name, vga_color(COLOR_LGREEN, COLOR_BLACK));
            vga_print(" [");
            vga_print_int(procs[cur_proc].remaining_time, vga_color(COLOR_YELLOW, COLOR_BLACK));
            vga_print("]");
            if (procs[cur_proc].remaining_time == 0) {
                vga_print_color(" <- termine", vga_color(COLOR_DGRAY, COLOR_BLACK));
            }
        } else {
            vga_print_color(" | idle", vga_color(COLOR_DGRAY, COLOR_BLACK));
        }
        vga_print("\n");
    }
    else if (k_strcmp(cmd, "time") == 0) {
        vga_print("Temps: ");
        vga_print_int(system_time, vga_color(COLOR_CYAN, COLOR_BLACK));
        vga_print(" ticks\n");
    }
    else if (k_strncmp(cmd, "run ", 4) == 0) {
        const char* args = cmd + 4;
        if (args[0] == '\0') {
            vga_print_color("Usage: run <nom> [burst]\n", vga_color(COLOR_LRED, COLOR_BLACK));
            vga_print_color("  Exemple: run myproc 8\n", vga_color(COLOR_DGRAY, COLOR_BLACK));
        } else {
            char name[12];
            uint32_t burst;
            parse_run_cmd(args, name, &burst);
            
            int pid = create_proc(name, burst);
            if (pid > 0) {
                vga_print_color("OK: ", vga_color(COLOR_LGREEN, COLOR_BLACK));
                vga_print(name);
                vga_print(" (PID ");
                vga_print_int(pid, vga_color(COLOR_CYAN, COLOR_BLACK));
                vga_print(", burst=");
                vga_print_int(burst, vga_color(COLOR_YELLOW, COLOR_BLACK));
                vga_print(")\n");
            } else {
                vga_print_color("Erreur: max processus atteint\n", vga_color(COLOR_LRED, COLOR_BLACK));
            }
        }
    }
    else if (k_strncmp(cmd, "kill ", 5) == 0) {
        int pid = 0;
        const char* p = cmd + 5;
        while (*p >= '0' && *p <= '9') { pid = pid * 10 + (*p - '0'); p++; }
        if (pid > 0) {
            kill_proc(pid);
            vga_print("PID ");
            vga_print_int(pid, vga_color(COLOR_WHITE, COLOR_BLACK));
            vga_print(" termine\n");
        } else {
            vga_print_color("Usage: kill <pid>\n", vga_color(COLOR_LRED, COLOR_BLACK));
        }
    }
    else if (k_strcmp(cmd, "exit") == 0 || k_strcmp(cmd, "quit") == 0) {
        running = 0;
    }
    else if (cmd[0] != '\0') {
        vga_print_color("Commande inconnue: ", vga_color(COLOR_LRED, COLOR_BLACK));
        vga_print(cmd);
        vga_print("\n");
    }
}

void handle_char(char c) {
    if (c == '\n') {
        vga_putchar('\n');
        cmd_buf[cmd_pos] = '\0';
        exec_cmd(cmd_buf);
        cmd_pos = 0;
        if (running) print_prompt();
    }
    else if (c == '\b') {
        if (cmd_pos > 0) {
            cmd_pos--;
            vga_putchar('\b');
        }
    }
    else if (cmd_pos < CMD_SIZE - 1) {
        cmd_buf[cmd_pos++] = c;
        vga_putchar_color(c, vga_color(COLOR_WHITE, COLOR_BLACK));
    }
}

// =============================================================================
// KERNEL MAIN
// =============================================================================
void kernel_main(void) {
    enable_cursor();
    vga_clear();
    
    // Logo simple
    vga_print_color("\n  _                      _       ", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color("__  __ _       _  ___  ____\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color(" | |    ___  __ _ _ __ _ __ (_)_  _", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color("|  \\/  (_)_ __ (_)/ _ \\/ ___|\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color(" | |   / _ \\/ _` | '__| '_ \\| \\ \\/ /", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color("| |\\/| | | '_ \\| | | | \\___ \\\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color(" | |__|  __/ (_| | |  | | | | |>  <", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color(" | |  | | | | | | | |_| |___) |\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color(" |_____\\___|\\__,_|_|  |_| |_|_/_/\\_\\", vga_color(COLOR_LCYAN, COLOR_BLACK));
    vga_print_color("|_|  |_|_|_| |_|_|\\___/|____/\n", vga_color(COLOR_LCYAN, COLOR_BLACK));
    
    vga_print("\n");
    vga_print_color("  Bienvenue dans LearnixMiniOS!\n", vga_color(COLOR_LGREEN, COLOR_BLACK));
    vga_print("  Tapez 'help' pour les commandes.\n");
    
    log_event("Systeme demarre");
    
    // Créer processus initiaux
    create_proc("init", 10);
    create_proc("shell", 8);
    
    print_prompt();
    
    while (running) {
        char c = keyboard_getchar();
        handle_char(c);
    }
    
    print_shutdown();
    while (1) { asm volatile("hlt"); }
}