#ifndef RTOS_START_H_
#define RTOS_START_H_
#include

void setPsp(uint32_t stack_top);

uint32_t* getPsp(void);

void setAsp(void);

#endif
