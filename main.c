#include "clock.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

#define PA0_MASK 0x00000001	// Uart0_Rx
#define PA1_MASK 0x00000002	// Uart0_Tx
void reboot();

// PC <-> tm4c 
void initUart0()
{
    // 1. ENABLE CLOCK
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);
    
    // 2. CONFIG GPIO Pins
    GPIO_PORTA_DEN_R |= (PA0_MASK | PA1_MASK);
    GPIO_PORTA_AFSEL_R |= (PA0_MASK | PA1_MASK);
    GPIO_PORTA_PCTL_R |= (GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
    GPIO_PORTA_DR2R_R |= PA1_MASK;

    // 3. CONFIG UART
    UART0_CTL_R = 0;
    UART0_CC_R = UART_CC_CS_SYSCLK;

    UART0_IBRD_R = 21;	// 40Mhz, 115200 baud
    UART0_FBRD_R = 45;
    UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
  
    // 4. CONFIG INTERRUPT
    UART0_ICR_R = 0xFFFF;	    		 // 4.1 Clear all interrupt
    UART0_IFLS_R = UART_IFLS_RX1_8;  		 // 4.2 Interrupt FIFO lvl 1/8
    UART0_IM_R = (UART_IM_RXIM | UART_IM_RTIM);  // Interrupt Mask
    
    NVIC_EN0_R |= (1 << 5);			 // 4.3 IRQ #5
    NVIC_PRI1_R &= ~0x0000E000;			 // RESET
    NVIC_PRI1_R |= 0x00004000;			 // Priority #2, 0x0000 (0100)000
    
    // 5. ENABLE
    UART0_CTL_R = (UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN);
}

// Display process(thread) status
void ps()
{
    
}

main()
{

initSystemClockTo40Mhz(void);

}
