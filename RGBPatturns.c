#include <msp430.h>
#include <stdlib.h>
#include "LEDController.h"
extern int LED_resolution;
unsigned long patturnIndex[] = {0,0,0,0};
unsigned long patturnTime[] = {0,0,0,0};
extern struct RGBLED LED[2];
double referenceWavelength;		// variable to store wavelength value for random
								// color mode.

/* This array is used to store the color patturns to be displayed by the LEDs.
 *
 * Defined as: patturns[number of patturns][number of LEDs in each patturn][length of longest patturn]
 *
 * All patturns will be displayed in a loop from the beginning.
 *
 * EACH LED'S COLOR PATTURN MUST END IN A '-1' TO SIGNIFY THE END OF THE ARRAY.
 *
 * Values are entered in pairs of integers: First is the wavelength of the color to be displayed
 * (in range [380, 781]); second is the duration for which the color is to be displayed (in milliseconds).
 * 
 * Alternatively, a random wavelength can be set by entering a '1' in place of an actual
 * wavelength value.
 * 
 * Double alternatively, a color shift can be displayed. This is done by entering a set of 
 * four numbers: The first is a '2' to signify a color shift, the next is the durration for
 * which the color shift is to take place (in milliseconds), and the last two are the starting
 * and ending wavelengths.
 *
 * notes:
 * - To turn off an LED, set wavelength value to '0' for consistancy.
 * - The largest possible durration value is 65535 (0xFFFF).
 * - Unique color values are only in wavelength range [380, 645]. values in range
 *   [646, 781] simply fade out the blue color.
 *
 * Programmers note: using pointers and malloc()ing space may be a better implementation. It would allow
 * each patturn to be a different length in memory. It would also allow each LEDs color patturn to be
 * different lengths in memory.
 */
int patturns[][2][13] = 
{
	{
		{1,1000, -1},
		{1,1000, -1}
	},
		{2,10000,380,680,2,1000,680,380, -1},
		{2,5000,380,680,2,5000,680,380, -1}
};

void updateColors(int patturn)
{
	int nArgs;		// Number of integer values in argument set

	/* Loop through LED array */
	int i;
	for (i=0; i < sizeof(LED)/sizeof(LED[0]); i++)
	{
		/* If a color shift */
		if(patturns[patturn][i][patturnIndex[i]] == 2)
		{
			/* Set color by updating LED struct's 'wavelength' field */
			LED[i].wavelength = CalculateColorShiftWavelength(patturns[patturn][i][patturnIndex[i]+1],
															  patturns[patturn][i][patturnIndex[i]+2],
															  patturns[patturn][i][patturnIndex[i]+3],
															  				  (double)patturnTime[i]);
			nArgs = 4;
		}
		/* If a random wavelength */
		else if (patturns[patturn][i][patturnIndex[i]] == 1)
		{
			/* If this is the first color update of the argument set */
			if (patturnTime[i] == 0)
				referenceWavelength = generateRandomWavelength();

			/* Set color by updating LED struct's 'wavelength' field */
			LED[i].wavelength = referenceWavelength;

			nArgs = 2;
		}
		else
		{
			/* Set color by updating LED struct's 'wavelength' field */
			LED[i].wavelength = patturns[patturn][i][patturnIndex[i]];

			nArgs = 2;
		}

		/* Increment patturnTime */
		patturnTime[i]++;

		/* Increase patturnIndex by nArgs and reset patturnTime if this color is done displaying */
		if (patturnTime[i] >= patturns[patturn][i][patturnIndex[i]+1]/COLOR_UPDATE_PERIOD)
		{
			patturnIndex[i] += nArgs;
			patturnTime[i] = 0;
		}

		/* Reset patturnIndex if at end of array */
		if (patturns[patturn][i][patturnIndex[i]] == -1)
		{
			patturnIndex[i] = 0;
		}
	}

}
/* 
 * Calculates and returns the wavelength value for a color shift based on 
 * current patturn time.
 */
double CalculateColorShiftWavelength(double transitionTime, double startWavelength,
										  double endWavelength, double currentTime)
{
	return (currentTime*COLOR_UPDATE_PERIOD/transitionTime) * (endWavelength-startWavelength) + startWavelength;
}

/* 
 * Returns a random wavlength value
 */
double generateRandomWavelength() 
{
	/* Return random integer within range [380,750] */
	return rand()%370+380;
}