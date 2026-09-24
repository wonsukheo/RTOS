#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "rtos.h"
#include "shell.h"
#include "uart0.h"
#include "clock.h"

#define PF3_MASK 0X00000008 // GREEN_LED
#define PF2_MASK 0X00000004 // BLUE_LED
#define PF1_MASK 0X00000002 // RED_LED

int main(void) 
{
    initSystemClockTo40Mhz();
    initGPIOPFLED();
    initUart0();
    
    shell();

    putsUart0("out of shell process\r\n");
    while(true);

    return 0;
}


void initGPIOPFLED(void)
{
    // ENABLE CLOCK
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    GPIO_PORTF_DEN_R |= (PF1_MASK | PF2_MASK | PF3_MASK);
    GPIO_PORTF_DIR_R |= (PF1_MASK | PF2_MASK | PF3_MASK);
    GPIO_PORTF_DR2R_R |= (PF1_MASK | PF2_MASK | PF3_MASK);

    GREEN_LED_BBADDR = 0;
    BLUE_LED_BBADDR = 0;
    RED_LED_BBADDR = 0;
}

void yield(void)
{

}
