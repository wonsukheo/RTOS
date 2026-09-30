#ifndef RTOS_START_H_
#define RTOS_START_H_

void setPsp(uint32_t stack_top);

uint32_t* getPsp(void);

void setAsp(void);

void launchThread(uint32_t stackTop, void (*thread)(void));

#endif
