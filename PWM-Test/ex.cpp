/*
Purpose: Test GPIO pins
Author: Josh Osborne

*/

#include <iostream>
#include <unistd.h>
#include "GPIO.h"
#include "PWM.h"

using namespace exploringBB;
using namespace std;

int main()
{
	PWM clock_rate("pwm_test_P8_19.15");
	clock_rate.setPeriod(1000);
	clock_rate.setDutyCycle(50.0f);
	clock_rate.setPolarity(PWM::ACTIVE_LOW);
	clock_rate.run();
	sleep(20);
	clock_rate.stop();	

	/*GPIO clock_rate(60);
	clock_rate.setDirection(OUTPUT);
	
	// Create a baud rate clock
   
	for (int i=0; i<1000000; i++)
	{
		clock_rate.setValue(HIGH);
		usleep(10);
		clock_rate.setValue(LOW);
		usleep(10);
		
	}*/
	

	//clock_rate.toggleOutput(100, 500);
	
}
