#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>

#define PB_MASK 0x7F // 0x0111.1111 PB0~PB6
#define PD_MASK 0X0F // 0x1111 PD0~PD3

#define RED_LED_BBADDR  *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 1*4))  //PF1
#define BLUE_LED_BBADDR *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 2*4))  //PF2
#define GREEN_LED_BBADDR *((volatile uint32_t*)(0x42000000 + (0x400253FC-0X40000000)*32 + 3*4))  //PF3

void initGPIO(void);
void getPressedButton(void);

#endif
