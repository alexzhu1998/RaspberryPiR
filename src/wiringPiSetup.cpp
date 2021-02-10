#include <wiringPi.h>
#include <stdio.h>
#include <Rcpp.h>
using namespace Rcpp;

// TODO:
// Set up for wiring Pi wrappers from the documentations
// http://wiringpi.com/reference/setup/
// http://wiringpi.com/reference/core-functions/


/*
void pinMode (int pin, int mode) ;
This sets the mode of a pin to either INPUT, OUTPUT, PWM_OUTPUT or GPIO_CLOCK.
Note that only wiringPi pin 1 (BCM_GPIO 18) supports PWM output and only wiringPi
 pin 7 (BCM_GPIO 4) supports CLOCK output modes.

This function has no effect when in Sys mode. If you need to change the pin mode,
then you can do it with the gpio program in a script before you start your program.

void pullUpDnControl (int pin, int pud) ;

This sets the pull-up or pull-down resistor mode on the given pin, which should
 be set as an input. Unlike the Arduino, the BCM2835 has both pull-up an down
 internal resistors. The parameter pud should be; PUD_OFF, (no pull up/down),
 PUD_DOWN (pull to ground) or PUD_UP (pull to 3.3v) The internal pull up/down
 resistors have a value of approximately 50KΩ on the Raspberry Pi.

This function has no effect on the Raspberry Pi’s GPIO pins when in Sys mode.
If you need to activate a pull-up/pull-down, then you can do it with the gpio
program in a script before you start your program.

void digitalWrite (int pin, int value) ;

Writes the value HIGH or LOW (1 or 0) to the given pin which must have been
previously set as an output.

WiringPi treats any non-zero number as HIGH, however 0 is the only
representation of LOW.

void pwmWrite (int pin, int value) ;

Writes the value to the PWM register for the given pin. The Raspberry Pi has
one on-board PWM pin, pin 1 (BMC_GPIO 18, Phys 12) and the range is 0-1024.
Other PWM devices may have other PWM ranges.

This function is not able to control the Pi’s on-board PWM when in Sys mode.

int digitalRead (int pin) ;

This function returns the value read at the given pin. It will be HIGH or LOW
(1 or 0) depending on the logic level at the pin.

analogRead (int pin) ;

This returns the value read on the supplied analog input pin. You will need to
register additional analog modules to enable this function for devices such as
the Gertboard, quick2Wire analog board, etc.

analogWrite (int pin, int value) ;

This writes the given value to the supplied analog pin. You will need to
register additional analog modules to enable this function for devices such as
the Gertboard.
*/

// [[Rcpp::export]]
int setUp() {
    return wiringPiSetup();
}

// [[Rcpp::export]]
void pinMode(ledpin,OUTPUT) {
    pinMode(ledpin,OUTPUT);
}




/*** R
setUp()
*/
