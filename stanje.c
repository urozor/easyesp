#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h> // Include WiringPi library!
// part of code from: https://learn.sparkfun.com/tutorials/raspberry-gpio/c-wiringpi-example
// Pin number declarations. We're using the Broadcom chip pin numbers.
// compile with wiringPi library:
// gcc stanje.c -o stanje -l wiringPi

int main( int argc, char *argv[] )
{
	int pinRed = 27; // Alarmna centrala J7 pin 5
	int stanjeRed = 9;
	int pinGreen = 29; // Alarmna centrala J7 pin 9
	int stanjeGreen = 9;
	int stevec = 0;

	if (argc > 2) {
		printf("\nPotreben argument: utripa <pin>\n");
		printf("0 - ne utripa, low\n");
		printf("1 - ne utripa, high\n");
		printf("2 - utripa\nUporabi ukaz sudo gpio readall in izberi wiringPi pin\n");
		exit(1);
	}
	// Setup stuff:
	//   wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers
	wiringPiSetup();

	pullUpDnControl(pinRed,PUD_OFF);
	pullUpDnControl(pinGreen,PUD_OFF);
	pinMode(pinRed, INPUT);      // Set pin as INPUT
	pinMode(pinGreen, INPUT);
	if(digitalRead(pinRed)==1) {
		stevec = stevec+1;
	}
	delay(120);
	if(digitalRead(pinRed)==1) {
		stevec = stevec+1;
	}
	delay(120);
	if(digitalRead(pinRed)==1) {
		stevec = stevec+1;
	}
	//    printf("zbrano:%d\n",stevec);

	if (stevec == 0)
	{
		stanjeRed=0; //LOW
	}
	else
	{
		if (stevec == 3)
		{
			stanjeRed=1;  //HIGH
		} else {
			stanjeRed=2;  //utripa
		}
	}


	stanjeGreen = digitalRead(pinGreen);

	if (stanjeGreen==1 & stanjeRed==1 ){printf("1");} // disarmed and not ready
	if (stanjeGreen==0 & stanjeRed==1 ){printf("2");} // disarmed and ready
	if (stanjeGreen==1 & stanjeRed==0 ){printf("3");} // armed away
	if (stanjeGreen==1 & stanjeRed==2 ){printf("4");} // armed stay or alarm memory (slow or rapid flash)

	return 0;
}
