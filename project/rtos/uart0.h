#ifndef UART0_H_
#define UART0_H_

#include <stdint.h>
#include <stdbool.h>

void initUart0(void);
void Uart0_Interrupt_Handler(void);
bool kbhit(void);
char getcUart0(void);
void getsUart0(char* str);
void putcUart0(char c);
void putsUart0(const char* str);
void putUintUart0(uint32_t val);

#endif 