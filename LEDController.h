/* @author Jonah Groendal
 * 5/27/16
 *
 */
#define LED_DISPLAY_PERIOD	500		// Sets the delay between displaying LEDs (in microseconds)
 
#define COLOR_UPDATE_PERIOD 10		// Sets the delay between color updates (in milliseconds)
 									// Maximum value is 32

void configureTimer0(int delay);
void configureTimer1(int delay);
void displayRGB(int ledNum);
void displayAllRGB();
void calculateRGB(double Wavelength, char RGB[]);
void updateColors(int patturn);
double CalculateColorShiftWavelength(double transitionTime, double startWavelength,
									     double endWavelength, double currentTime);
double generateRandomWavelength();

struct RGBLED {
	double referenceWavelength;	// Current wavelength (color) setting. For reference
								// by interrupt Timer0_A0 ONLY
	double wavelength;			// New wavelength (color) for RGB values. Use this to
								// change LED color.
	char RGB[3];				// Current RGB values (is a function of wavelength
								// and calculated with calculateRGB())
	char RGBGPIO[3];			// Saves the bit value of the pin associated with
								// R,G,B GPIO respectively
};
