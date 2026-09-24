#ifndef SHELL_H_
#define SHELL_H_

#include <stdbool.h>
#include <stdint.h>

void shell(void);
bool isBackground(USER_DATA* data);

void ps(void);
void ipcs(void);
void kill(uint32_t pid);
void pkill(const char* pname);
void pidof(const char* pname);
void pi(bool onOff);
void preempt(bool onOff);
void sched(bool prioRR);

#endif