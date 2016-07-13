/* @author Jonah Groendal
 * 5/27/16
 *
 */

#include <msp430.h>
#include <stdlib.h>
#include "LEDController.h"


int LED_resolution = 30;
struct RGBLED LED[NUM_LEDS];
int CURRENT_PATTURN_SELECTION = 0;
int NUM_PATTURNS = 3;


int main(void) {

	/* Hold watchdog timer */
	WDTCTL = WDTPW + WDTHOLD;

	/* Set CPU to 16MHz */
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL  = CALDCO_16MHZ;
	
	/* Set GPIO; initialize them to off */
	P1DIR |=  0b11110111;
	P2DIR |=  0b00011111;
	P1OUT &= ~0b11110111;
	P2OUT &= ~0b00011111;
	P1OUT |= BIT3;

	/* Assign GPIO pin to each LED's RGBGPIO values */
	LED[0].RGBGPIO[0] = BIT0; LED[0].P1orP2[0] = 1;
	LED[0].RGBGPIO[1] = BIT1; LED[0].P1orP2[1] = 1;
	LED[0].RGBGPIO[2] = BIT2; LED[0].P1orP2[2] = 1;
	LED[1].RGBGPIO[0] = BIT4; LED[1].P1orP2[0] = 1;
	LED[1].RGBGPIO[1] = BIT5; LED[1].P1orP2[1] = 1;
	LED[1].RGBGPIO[2] = BIT6; LED[1].P1orP2[2] = 1;
	LED[2].RGBGPIO[0] = BIT7; LED[2].P1orP2[0] = 1;
	LED[2].RGBGPIO[1] = BIT0; LED[2].P1orP2[1] = 2;
	LED[2].RGBGPIO[2] = BIT1; LED[2].P1orP2[2] = 2;
	LED[3].RGBGPIO[0] = BIT2; LED[3].P1orP2[0] = 2;
	LED[3].RGBGPIO[1] = BIT3; LED[3].P1orP2[1] = 2;
	LED[3].RGBGPIO[2] = BIT4; LED[3].P1orP2[2] = 2;
	//LED[4].RGBGPIO[0] = BIT5; LED[4].P1orP2[0] = 2;
	//LED[4].RGBGPIO[1] = BIT6; LED[4].P1orP2[1] = 2;
	//LED[4].RGBGPIO[2] = BIT7; LED[4].P1orP2[2] = 2;

	/* Enable pullup resistor for button */
	P1REN |= BIT3;
	/* Interrupt from high to low */
    //P1IES |= BIT3;
    /* Clear interrupt flag */
    //P1IFG &= ~BIT3;
	/* Interrupt on input pin P1.3 */
	//P1IE |= BIT3 ;

	/* Configure timer 0 to trigger an interrupt every *calibrated value* microseconds */
	int updatePeriod = 60*LED_resolution*(sizeof(LED)/sizeof(LED[0]));
	configureTimer0(updatePeriod);

	/* Configure timer 1 to trigger an interrupt every COLOR_UPDATE_PERIOD millisiconds */
	configureTimer1(COLOR_UPDATE_PERIOD);

	//__enable_interrupt();
	__bis_SR_register(GIE);
/*
	LED[0].wavelength = 450;
	calculateRGB(LED[0].wavelength, LED[0].RGB);
	LED[1].wavelength = 450;
	calculateRGB(LED[1].wavelength, LED[1].RGB);
*/
	while(1){}
}

/* Configures Timer1_A0 to trigger an interrupt after every delay interval. 
 * 
 * Paramaters:
 * - int delay: number of MICROseonds to delay interrupt
 */
void configureTimer0(int delay)
{
	TA0CCR0  |= 2*delay;				// Delay interrupt for 2*delay clock cycles.
										// 16 beccause CPU is set at 16MHz and ID_3 
										// divides clock cycles by 8.
	TA0CCTL0 |= CCIE;					// Enable interrupt
	TA0CTL   |= TASSEL_2 + MC_1 + ID_3;	// Timer selection and mode: SMCLK, up mode
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
__interrupt void Timer0_A0 (void) 
{
	/*****************************************************************
	 * CALCULATE AND DISPLAY RGB VALUES
	 ****************************************************************/
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
__interrupt void Timer1_A0 (void) 
{
	/* CHECK FOR BUTTON PRESS
	 * If button is pushed down (P1.3), increment patturn selection */
	if (!(BIT3 & P1IN))
	{
		/* Increment CURRENT_PATTURN_SELECTION */
		if (CURRENT_PATTURN_SELECTION >= NUM_PATTURNS-1)
			CURRENT_PATTURN_SELECTION = 0;
		else
			CURRENT_PATTURN_SELECTION++;

		/* Hold while button is pushed down */
		while(!(BIT3 & P1IN)) {}
		__delay_cycles(160000);
	}
	updateColors(CURRENT_PATTURN_SELECTION);
}
