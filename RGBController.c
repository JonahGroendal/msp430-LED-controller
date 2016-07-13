/* @author Jonah Groendal
 * 5/27/16
 *
 */

#include <msp430.h>
#include "LEDController.h"
extern unsigned int LED_resolution;
extern struct RGBLED LED[NUM_LEDS];

/* Displays color and intensity for LED[ledNum].
 *
 * Takes about 1*LED_resolution milliseconds @ 1MHZ to fully display a color
 */
void displayRGB(int ledNum) 
{
    int i;
    for (i=0; i< LED_resolution; i++)
    {
    	if (LED[ledNum].RGB[0] > (i % LED_resolution)) {
    		P1OUT |= LED[ledNum].RGBGPIO[0];
    	}
    	else {
    		P1OUT &= ~LED[ledNum].RGBGPIO[0];
    	}
    	if (LED[ledNum].RGB[1] > (i % LED_resolution)) {
    		P1OUT |= LED[ledNum].RGBGPIO[1];
    	}
    	else {
    		P1OUT &= ~LED[ledNum].RGBGPIO[1];
    	}
    	if (LED[ledNum].RGB[2] > (i % LED_resolution)) {
    		P1OUT |= LED[ledNum].RGBGPIO[2];
    	}
    	else {
    		P1OUT &= ~LED[ledNum].RGBGPIO[2];
    	}
    }

    // Turn off LEDs for color accuracy.
    // If an RGB value is '1', that color will stay on if it isn't
    // explicitly turned off.
    P1OUT &= ~LED[ledNum].RGBGPIO[0];
    P1OUT &= ~LED[ledNum].RGBGPIO[1];
    P1OUT &= ~LED[ledNum].RGBGPIO[2];
}

/* Updates all LEDs at once rather than one at a time with displayRGB(). This
 * allows all LEDs to be on at the same time, allowing for a greater total
 * brightness of the LEDs.
 * 
 * Takes about (size of LED array * LED_resolution) milliseconds @ 1MHZ to fully
 * display a color (based on timing of displayRGB())
 *
 * Takes about .05 * size of LED array * LED_RESOLUTION @ 1MHz @ 16MHz
 */
void displayAllRGB() 
{
    int i, ledNum, j;

    /* Loop through LED_resolution. This is done quickly to create color
       intensity through PWM */
    for (i=0; i< LED_resolution; i++)
    {
        /* Loop through LEDs */
        for (ledNum=0; ledNum < sizeof(LED)/sizeof(LED[0]); ledNum++)
        {
            /* Loop through RGB */
            for (j=0; j<3; j++)
            {
                /* Check if LED color is on pin P1.x or P2.x */
                switch(LED[ledNum].P1orP2[j]) {
                    case 1:
                        if (LED[ledNum].RGB[j] > (i % LED_resolution)) {
                            P1OUT |= LED[ledNum].RGBGPIO[j];
                        } else {
                            P1OUT &= ~LED[ledNum].RGBGPIO[j];
                        }
                        break;
                    case 2:
                        if (LED[ledNum].RGB[j] > (i % LED_resolution)) {
                            P2OUT |= LED[ledNum].RGBGPIO[j];
                        } else {
                            P2OUT &= ~LED[ledNum].RGBGPIO[j];
                        }
                        break;
                }
            }
        }
    }
    // Turn off LEDs for color accuracy.
    // If an RGB value is '1', that color will stay on if it isn't
    // explicitly turned off.
    for (ledNum=0; ledNum < sizeof(LED)/sizeof(LED[0]); ledNum++)
    {   
        /* Loop through RGB */
        int j;
        for (j=0; j<3; j++)
        {
            /* Check if LED color is on pin P1.x or P2.x */
            switch(LED[ledNum].P1orP2[j]) {
                    case 1:
                        P1OUT &= ~LED[ledNum].RGBGPIO[j];
                        break;
                    case 2:
                        P2OUT &= ~LED[ledNum].RGBGPIO[j];
                        break;
            }
        }
    }
}

/* Calculates the RGB values for a given wavelength
 * 
 * Paramaters:
 * - double Wavelength: Input paramater out of which the RGB
 * values are calculated.
 * - char RGB[]: Array of three byte values (R,G,B respectively) passed by the caller.
 * "output" of this function is placed into this array.
 *
 * Takes ~12ms @ 1MHz to calculate RGB values
 */
void calculateRGB(double Wavelength, char RGB[])
{
    double factor;
    double Red,Green,Blue;

    /* Calculate RGB intensities (as a decimal number between 0 and 1) */
    if((Wavelength >= 380) && (Wavelength<440)){
        Red = -(Wavelength - 440) / (440 - 380);
        Green = 0.0;
        Blue = 1.0;
    }else if((Wavelength >= 440) && (Wavelength<490)){
        Red = 0.0;
        Green = (Wavelength - 440) / (490 - 440);
        Blue = 1.0;
    }else if((Wavelength >= 490) && (Wavelength<510)){
        Red = 0.0;
        Green = 1.0;
        Blue = -(Wavelength - 510) / (510 - 490);
    }else if((Wavelength >= 510) && (Wavelength<580)){
        Red = (Wavelength - 510) / (580 - 510);
        Green = 1.0;
        Blue = 0.0;
    }else if((Wavelength >= 580) && (Wavelength<645)){
        Red = 1.0;
        Green = -(Wavelength - 645) / (645 - 580);
        Blue = 0.0;
    }else if((Wavelength >= 645) && (Wavelength<781)){
        Red = 1.0;
        Green = 0.0;
        Blue = 0.0;
    }else{
        Red = 0.0;
        Green = 0.0;
        Blue = 0.0;
    }

    /* Let intensity fall off near vision limits */
    if((Wavelength >= 380) && (Wavelength<420)){
        factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380);
    }else if((Wavelength >= 420) && (Wavelength<701)){
        factor = 1.0;
    }else if((Wavelength >= 701) && (Wavelength<781)){
        factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700);
    }else{
        factor = 0.0;
    }

    /* Convert RGB intensities to integers in range [0,LED_resolution] */
    int R = Red * LED_resolution;
    int G = Green * LED_resolution;
    int B = Blue * LED_resolution;

    /* Set the RGB[] array to calculated intensities */
    RGB[0] = R;
    RGB[1] = G;
    RGB[2] = B;
}
