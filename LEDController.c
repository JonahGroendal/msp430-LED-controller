/* @author Jonah Groendal
 * 5/27/16
 *
 */

#include <msp430.h>
#include <stdlib.h>
#include "LEDController.h"

unsigned int LED_resolution = 20;
struct RGBLED LED[2];


int main(void) {

	/* Hold watchdog timer */
	WDTCTL  = WDTPW | WDTHOLD;

	/* Set CPU to 16MHz */
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL  = CALDCO_16MHZ;
	
	/* Enable GPIO to output; initialize them to off */
	P1DIR  =  0b11111001;
	P1OUT &= ~0b11111001;

	/* Assign GPIO pin to each LED's RGBGPIO values */
	LED[0].RGBGPIO[0] = BIT0;
	LED[0].RGBGPIO[1] = BIT3;
	LED[0].RGBGPIO[2] = BIT4;
	LED[1].RGBGPIO[0] = BIT5;
	LED[1].RGBGPIO[1] = BIT6;
	LED[1].RGBGPIO[2] = BIT7;

	/* Configure timer 0 to trigger an interrupt every LED_DISPLAY_PERIOD microseconds */
	configureTimer0(LED_DISPLAY_PERIOD);

	/* Configure timer 1 to trigger an interrupt every COLOR_UPDATE_PERIOD millisiconds */
	configureTimer1(COLOR_UPDATE_PERIOD);

	__enable_interrupt();
	
	while(1){}
}

/* Configures Timer1_A0 to trigger an interrupt after every delay interval. 
 * 
 * Paramaters:
 * - int delay: number of MICROseonds to delay interrupt
 */
void configureTimer0(int delay)
{
	TA0CCR0  |= 16*delay;				// Delay interrupt for 16*delay clock cycles.
										// 16 beccause CPU is set at 16MHz.
	TA0CCTL0 |= CCIE;					// Enable interrupt
	TA0CTL   |= TASSEL_2 + MC_1;		// Timer selection and mode: SMCLK, up mode
}

/* Configures Timer1_A0 to trigger an interrupt after every delay interval. 
 * 
 * Paramaters:
 * - int delay: number of MILLIseonds to delay interrupt
 */
void configureTimer1(int delay)
{
	TA1CCR0  |= 2000*delay;				// Delay interrupt for 2000*delay clock cycles.
										// 2000 since CPU is set at 16MHz, ID_3 divides
										// clock cycles by 8, and delay is in ms.
	TA1CCTL0 |= CCIE;					// Enable interrupt
	TA1CTL   |= TASSEL_2 + MC_1 + ID_3;	// Select Timer, mode, and modifier: SMCLK, up mode, /8
}

/* TIMER0 INTERRUPT
 *
 * Interrupts rutienly to update all of the LEDs' wavelengths and RGB[] values.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void) {
	int i;
	for (i=0; i < sizeof(LED)/sizeof(LED[0]); i++)
	{
		// Check if RGB values need to be calculated
		if(LED[i].referenceWavelength != LED[i].wavelength)
		{
			//Calculate RGB values and return them to the LED struct's RGB array
			calculateRGB(LED[i].wavelength, LED[i].RGB);

			// Set LED struct's reference variable equal to newly calculated wavelength value
			LED[i].referenceWavelength = LED[i].wavelength;
		}
	}
	// Display set colors on all LEDs
	displayAllRGB();
}
/* TIMER0 INTERRUPT
 *
 * Interrupts rutienly to update all the LEDs' colors, which are set by the
 * patturns[][][] array.
 */
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void) {
	updateColors(1);
}
