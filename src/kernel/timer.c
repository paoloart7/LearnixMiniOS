#include <timer.h>

static uint32_t timer_ticks = 0;

void init_timer(void) {
    timer_ticks = 0;
}

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

void timer_tick(void) {
    timer_ticks++;
}

void log_event(const char* event, uint32_t data) {
    (void)event;
    (void)data;
    // Journalisation simple
}