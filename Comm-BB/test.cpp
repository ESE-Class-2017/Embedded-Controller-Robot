/*
Purpose: Beagle Bone Weather Station
	- Serial Communication
	- Reads RTD and sends the temperatures
Author: Josh Osborne
*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "comm.h"
#include "queue.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

#define DATA_LENGTH 10
#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

void get_data();
int average();
int readAnalog(int number);
double calc_temp(int adcVal);

// Globale Variable to save time
int data[DATA_LENGTH];

int main()
{
	int num;
	
	unsigned int i;
	unsigned int j;
	//int data[DATA_LENGTH];
	Serial_Comm comm1;
	char port[] = "/dev/ttyO4";

	comm1.Open_Port(port);
	comm1.Initialize_Port();

	/* initialize random seed: */
	srand (time(NULL));

	// Start thread to constantly read temperature data
	//std::thread temperature (get_data);	

	// Generate random numbers to represent analog data
	j = 0;
	for(i = 0; i < DATA_LENGTH; i++)
	{
		/* generate secret number between 1 and 4096: */
		num = rand() % 4096 + 1;
		cout << "num: " << num << endl;
		data[j] = num;
		j++;
	}

	

	
	cout << endl;

	
	/* Continuous Transmission of Data */
	while(1)
	{
		// Unique Start Character
		comm1.Send_Data(0xDEAD);
		// Seperator
		comm1.Send_Data(" ");
		// Sending of the data
		for(i=0; i < DATA_LENGTH; i++)
		{
			comm1.Send_Data(data[i]);
			comm1.Send_Data(" ");
		}	
		//Unique Stop Character
		comm1.Send_Data(0xBEEF);
		comm1.Send_Data(" ");

		usleep(100000);
	}


	//std::cout << comm1.Read_Data() << std::endl;
	sleep(5);
	//temperature.join();
	comm1.Close_Port();
	return 0;
}

/*
Purpose: Convert ADC value to temperature in celcuis
Author: Bryan
*/
double calc_temp(int adcVal)
{

	double temp ,Vdiff, adcTemp, gain = 3.94, VperDeg = 3.9, vref =2.5, Rbridge = 1000;

	// adcVal is the value from the ADC

	adcTemp = (adcVal/4095) *1.8; 

	Vdiff = (adcTemp/gain) + vref;

	temp = ((((Vdiff)/(5- Vdiff)))-1)*Rbridge/VperDeg;

	return temp;
	
}

/*
Purpose: Get the average of the data 
*/
int average()
{
	int i;
	int temp = 0;

	// Make a copy instead of a Mutex Lock
	int temp_data[DATA_LENGTH];

	// I am aware that data can change in the loop but this is the best choice for time
	for(i = 0; i < DATA_LENGTH; i++)
		temp_data[i] = data[i];

	for(i = 0; i < DATA_LENGTH; i++);
		temp += temp_data[i];

	temp = temp / LENGTH;

	return temp;
}

/*
Purpose: Get the temperature data in a thread
*/
void get_data()
{
	int j;
	int ATD_Value;

	while(1)
	{
		// Read analog value
		ATD_Value = readAnalog(0);
		data[j] = ATD_Value;
		j = ( j + 1 ) % DATA_LENGTH; 
	}
}

/*
Purpose: Read the file that will be the analog data value
*/
int readAnalog(int number)
{
	std::stringstream ss;
	std::fstream fs;

	ss << LDR_PATH << number << "_raw";

	fs.open(ss.str().c_str(), std::fstream::in);
	fs >> number;
	fs.close();

	return number;
}


