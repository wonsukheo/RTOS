#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "clock.h"
#include "rtos.h"
#include "rtos_start.h"
#include "shell.h"
#include "uart0.h"
#include "gpio.h"
#include "commandTerminalInterface.h"


// System Control Block(SCB) Registers
#define NVIC_INT_CTRL_R  *((volatile uint32_t*)0xE000ED04)
#define NVIC_CFG_CTRL_R *((volatile uint32_t*)0xE000ED14)
#define NVIC_SYS_HND_CTRL_R *((volatile uint32_t*)0xE000ED24)
#define NVIC_FAULT_STAT_R *((volatile uint32_t*)0xE000ED28)
#define NVIC_HFAULT_STAT_R *((volatile uint32_t*)0xE000ED2C)
#define NVIC_MM_ADDR_R *((volatile uint32_t*)0xE000ED34)
#define NVIC_FAULT_ADDR_R *((volatile uint32_t*)0xE000ED38)

#define PID 1
#define STACK_TOP 0X20008000

#define PF3_MASK 0X00000008 // GREEN_LED
#define PF2_MASK 0X00000004 // BLUE_LED
#define PF1_MASK 0X00000002 // RED_LED

int main(void) 
{
    initSystemClockTo40Mhz();
    
    initGPIO();		// PortB- PushButton, PortD- LED, PortF- on board LED
    initUart0();
    initHandler();

    setPsp((uint32_t*)STACK_TOP);
    setAsp();

    putsUart0("RTOS memory && Fault Test Ready\r\n");
    
    while (true) {			// gpio-pb-exception polling
        uint8_t button = getPressedButton();
 
        if (button) {
            if (button & (1 << 0)) {	// PB0: Bus fault
                triggerBusFault();
            }
            if (button & (1 << 1)) {	// PB1: Usage fault
                triggerUsageFault();
            } 
            if (button & (1 << 2)) {	// PB2: PendSV
                triggerPendSV();
            }
            if (button & (1 << 3)) {	// PB3: MPU Data fault
                triggerMPUDataFault();	
            }
            if (button & (1 << 4)) {
                triggerMPUInstFault();	// PB4: MPU Instruction fault
            }
            if (button & (1 << 5)) {
                triggerHardFault();	// PB5: Hard Fault
            }
            if (button & (1 << 6)) {
               				// tbd
            }
        }

    }
     
    shell();

    putsUart0("out of shell process\r\n");
    while(true);

    return 0;
}



void yield(void)
{

}

void initHandler(void)
{
    // 16-MPU, 17-Bus, 18- Usage 
    NVIC_SYS_HND_CTRL_R |= (1 << 16) | (1 << 17) | (1 << 18);
    
    // unaligned access trap enable
    NVIC_CFG_CTRL_R |= 1 << 3;
}

void printStackDump(uint32_t* pStack)
{
    putsUart0(" R0 : "); printHex32bit(*pStack);
    putsUart0(" R1 : "); printHex32bit(*++pStack);
    putsUart0(" R2 : "); printHex32bit(*++pStack);
    putsUart0(" R3 : "); printHex32bit(*++pStack);
    putsUart0(" R12 : "); printHex32bit(*++pStack);
    putsUart0("\r\n");
    
    putsUart0(" LR : "); printHex32bit(*++pStack);
    putsUart0(" PC : "); printHex32bit(*++pStack);
    putsUart0(" xPSR : "); printHex32bit(*++pStack);
    putsUart0("\r\n");
}

uint32_t getInstructionSize(uint32_t PCreg)
{
    // Thumb opcode(bit0) mask with 0, so access 2byte only

    uint16_t opcode = *((uint16_t*)(PCreg & ~1))
    
    uint16_t upper5 = (opcode >> 11) & 0x1F;
    
    // if (upper5 bits == 11101, 11110, 11111) then 32bit

    switch (upper5) {
        case 0x1D:
        case 0x1E:
        case 0x1F:		//fall thru
            return 4;
        default:
            return 2;

    }

}

// --------------------------------------------
// BUS FAULT (PB0): access unclocked gpio port 
// --------------------------------------------
void triggerBusFault(void)
{
    volatile uint32_t* unclocked_port = (volatile uint32_t*)0x400063FC;

    volatile uint32_t error = *unclocked_port;
}

void BusFault_Handler(void)
{
    uint32_t* psp = getPsp();
    
    putsUart0("Bus fault in thread PID_1\r\n"); 
    
    printStackDump(psp);

    // sync exception, jump to next instruction
    psp[6] += getInstructionSize(psp[6]);
}

// --------------------------------------------
// USAGE FAULT (PB1): access unaligned memory 
// --------------------------------------------
void triggerUsageFault(void)
{
    // 32bit data type, but 2000.0101 addr, 4 memory banks are not in same row
    volatile uint32_t* unaligned_ptr = (volatile uint32_t*)0x20000101;
    
    volatile uint32_t error = *unaligned_ptr; 
}

void UsageFault_Handler(void)
{
    uint32_t* psp = getPsp();
    
    putsUart0("Usage fault in thread PID_1\r\n"); 
    
    printStackDump(psp);
    // sync exception, jump to next instruction
    psp[6] += getInstructionSize(psp[6]);
}

// --------------------------------------------
// MPU DATA FAULT (PB2): Unprivileged || subregion OFF
//     INSTRUCTION FAULT (PB3): XN region de-ref as function ptr
// --------------------------------------------
void triggerMPUDataFault(void) 
{ 
    volatile uint32_t* restricted_sram = (volatile uint32_t*)0x20007000;

    *restricted_sram = 0x11111111;
}

void triggerMPUInstFault(void)
{
    // thumb bit(bit 0) keep it to 1

    void (*eXecuteNever)(void) = (void (*)(void))0x20007001;
    eXecuteNever();
}

void MPU_Handler(void)
{
    uint32_t* psp = getPsp();
    
    putsUart0("MPU fault in thread PID_1, FaultStat: 0x");
    printHex32bit((uint32_t)NVIC_FAULT_STAT_R & 0xFF); 
    putsUart0("\r\n");

    // DERR
    if (NVIC_FAULT_STAT_R & (1 << 7)) {		// MMARV
        putsUart0("Offending Data Addr: 0x");
        printHex32bit((uint32_t)NVIC_MM_ADDR_R); 
        putsUart0("\r\n");
    }
    
    printStackDump(psp);

    // MPU pending(MEMP) clear
    NVIC_SYS_HND_CTRL_R &= ~(1 << 13);

    // pendSV trigger
    NVIC_INT_CTRL_R |= (1 << 28);

    // sync fault
    psp[6] += getInstructionSize(psp[6]);
}

// --------------------------------------------
// pendSV (PB4): access unclocked gpio port 
// --------------------------------------------
void triggerPendSV(void)
{
    NVIC_INT_CTRL_R |= (1 << 28);	// PENDSET (1)
}

void PendSV_Handler(void)
{
    uint32_t* psp = getPsp();
    
    putsUart0("pendSV in thread PID_1\r\n"); 
    
    // MPU DERR or IERR (0011)
    if (NVIC_FAULT_STAT_R & 0x03) {	
        NVIC_FAULT_STAT_R = 0x03;	// Write 1 to clear
        putsUart0("memory protection called pendSV handler\r\n"); 
    }

    // async exception
    printStackDump(psp);
}

// --------------------------------------------
// Hard Fault (PB5): UsageFault OFF -> unaligned access -> escalate to hf
// --------------------------------------------
void triggerHardFault(void)
{
    NVIC_SYS_HND_CTRL_R &= ~(1 << 18);

    volatile uint32_t* unaligned = (volatile uint32_t*)0x20000101;
    volatile uint32_t error = *unaligned;
}

void HardFault_Handler(void)
{
    uint32_t* psp = getPsp();

    putsUart0("Hard fault in thread PID_1, HFaultStat: 0x");
    printHex32bit((uint32_t)NVIC_HFAULT_STAT_R & 0xFF); 
    putsUart0("\r\n");

    printStackDump(psp);
    // UsageFault back ON
    NVIC_SYS_HND_CTRL_R |= (1 << 18);

    // sync jump
    psp[6] += getInstructionSize(psp[6]);
}
