#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "rtos.h"	// yield() && LED_BB_ADDR

// Hardware Pin Mask
#define PA0_MASK 0x00000001	// Uart0_Rx
#define PA1_MASK 0x00000002	// Uart0_Tx

// Software Ring Buffer (private)
#define BUFFER_SIZE 64

static volatile char rx_buffer[BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

// PC <-> tm4c 
void initUart0(void)
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

void Uart0_Interrupt_Handler(void)
{
    // DEBUG
    BLUE_LED_BBADDR = 1;

    // 1. SNAPSHOT interrupt status register
    volatile uint32_t status = UART0_MIS_R;
    
    // 2. CLEAR FLAG register
    UART0_ICR_R = (UART_ICR_RXIC | UART_ICR_RTIC);
    
    // 3. HANDLE RX Interrupt, RX FIFO(1/8 FULL) or RT(Receive Timeout)
    if (status & (UART_MIS_RXMIS | UART_MIS_RTMIS)) {
        while ((UART0_FR_R & UART_FR_RXFE) == 0) {	// 3.1 WHILE RX FIFO IS NOT EMPTY
            char readChar = UART0_DR_R & 0xFF;
            uint8_t next_head = (rx_head + 1) & (BUFFER_SIZE - 1); // wraps index 
            
            if (next_head != rx_tail) {			// 3.2 If FULL, Discard incoming data
                rx_buffer[rx_head] = readChar;
                rx_head = next_head;
            }
        }
    }

    BLUE_LED_BBADDR = 0;
}

bool kbhit(void)
{
    return (rx_head != rx_tail);
}

// Caller ensure khbit() is true
char getcUart0(void)
{
    char c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) & (BUFFER_SIZE - 1);    // wraps index
    
    return c;
}

void getsUart0(char* str)
{
    uint8_t count = 0;
    
    while (true) {
        // if no more char to read in buffer, yield
   	while (!kbhit()) {
   	    yield();
        }
    
        char c= getcUart0();
        
        if (c == 8 || c == 127) {	// 8-backspace, 127-delete
            if (count > 0) {
                count--;
                putcUart0('\b');	// visualize echo
		putcUart0(' ');
		putcUart0('\b');
            }
        } 
        else if (c == 13 || c == 10) { // 10-enter, 13-return
            str[count] = '\0';

            putcUart0('\r');		 // echo
            putcUart0('\n');
            break;			 // end of input, return to shell
        }
        else if (c >= 32) {
            if (count < BUFFER_SIZE) {
                if (c >= 65 && c <= 90) {
                    c |= 32;
                }

                str[count++] = c;
                putcUart0(c);		 // echo
            }
        }
    }
}

// Cooperative
void putcUart0(char c)
{
    // if hw buffer is full, yield()
    while (UART0_FR_R & UART_FR_TXFF) {
        yield();
    }
    
    UART0_DR_R = c;
}

void putsUart0(const char* str)
{
    while (*str != '\0') {
        putcUart0(*str++);
    }    
}

void putUintUart0(uint32_t val)
{
    char str[11];
    int8_t i = 0;
    
    if (val == 0) {
        putcUart0('0');
        return;
    }
    // write in reverse
    while (val > 0) {
        str[i++] = (val % 10) + '0';
        val /= 10;
    }
    // print in reverse
    while (--i >= 0) {
        putcUart0(str[i]);
    }

}

