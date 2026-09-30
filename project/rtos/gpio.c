#include "gpio.h"
#include "clock.h"
#include "tm4c123gh6pm.h"

/* port B[0:6] <- x7 pb
   port D[0:3] -> x4 LED
   port F[1:3] -> on_board LED */
void initGPIO(void)
{
    // ENABLE CLOCK
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R1;
    _delay_cycles(3);

    GPIO_PORTB_DEN_R |= PB_MASK;
    GPIO_PORTB_DIR_R &= ~PB_MASK;
    GPIO_PORTB_PUR_R |= PB_MASK;
    GPIO_PORTB_IS_R  &= ~0x7F;  // Edge sensitive (not level)
    GPIO_PORTB_IBE_R &= ~0x7F;  // Controlled by IEV (single edge, not both)
    GPIO_PORTB_IEV_R &= ~0x7F;  // Falling edge (active-low press)
    GPIO_PORTB_ICR_R  =  0x7F;  // Clear any stale edge flags

    GPIO_PORTD_DEN_R |= PD_MASK;
    GPIO_PORTD_DIR_R |= PD_MASK;
    GPIO_PORTD_DR2R_R |= PD_MASK;

    GPIO_PORTF_DEN_R |= (PF1_MASK | PF2_MASK | PF3_MASK);
    GPIO_PORTF_DIR_R |= (PF1_MASK | PF2_MASK | PF3_MASK);
    GPIO_PORTF_DR2R_R |= (PF1_MASK | PF2_MASK | PF3_MASK);

    GREEN_LED_BBADDR = 0;
    BLUE_LED_BBADDR = 0;
    RED_LED_BBADDR = 0;
}

uint8_t getPressedButton(void)
{
    uint8_t pressed = GPIO_PORTB_RIS_R & 0x7F;

    if (pressed) {
        // clear
        GPIO_PORTB_ICR_R = pressed;
        // 20ms debounce
        _delay_cycles(800000);
    }

    return pressed;
}
