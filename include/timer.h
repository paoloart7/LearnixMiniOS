#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void init_timer(void);
uint32_t get_timer_ticks(void);
void timer_tick(void);
void log_event(const char* event, uint32_t data);

#endif