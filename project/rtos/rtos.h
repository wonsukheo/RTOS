#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include <stdbool.h>

#define RED_LED_BBADDR  *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 1*4))  //PF1
#define BLUE_LED_BBADDR *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 2*4))  //PF2
#define GREEN_LED_BBADDR *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 3*4))  //PF3

void initGPIOPFLED(void);
void yield(void);
void printStackDump(uint32_t* pStack);
uint32_t getInstructionSize(uint32_t PCreg);
void initHandler(void);
void gpioStart(void);

void triggerBusFault(void);
void triggerUsageFault(void);
void triggerMPUInstFault(void);
void triggerMPUDataFault(void);
void triggerPendSV(void);
void triggerHardFault(void);

void HardFault_Handler(void);
void PendSV_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void MPU_Handler(void);

#endif
