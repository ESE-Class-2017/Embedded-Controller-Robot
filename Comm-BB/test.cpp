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
void data_transmit(Serial_Comm &comm1);
void get_humidity(Serial_Comm &comm2);


// Globale Variable to save time
int data[DATA_LENGTH];

int main()
{
	int num;
	
	unsigned int i;
	unsigned int j;
	//int data[DATA_LENGTH];
	Serial_Comm comm1;
	Serial_Comm comm2;
	char port1[] = "/dev/ttyO4";
	char port2[] = "/dev/ttyO1";

	comm1.Open_Port(port1);
	comm1.Initialize_Port(B1200);

	comm2.Open_Port(port2);
	comm2.Initialize_Port(B9600);

	/* initialize random seed: */
	srand (time(NULL));

	// Start thread to constantly read temperature data
	//std::thread t1 (get_data);
	// Start thread for transmitting data
	std::thread t2 (data_transmit, std::ref(comm1));
	// Start thread for getting humidity data
	//std::thread t3 (get_humidity, std::ref(comm2));	

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

	
while(1);

	sleep(5);
	//t1.join();
	t2.join();
	//t3.join();
	comm1.Close_Port();
	return 0;
}

void get_humidity(Serial_Comm &comm2)
{
	if(!comm2.Read_Empty())
		std::cout << comm2.Read_Data() << std::endl;
}

/*
Purpose: Convert ADC value to temperature in celcuis
Author: Bryan
*/

void data_transmit(Serial_Comm &comm1)
{
	int i;

	/* Continuous Transmission of Data */
	while(1)
	{
		// Unique Start Character
		comm1.Send_Data(0xFEED);
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

		// Unique Start Character
		comm1.Send_Data(0xDEAD);
		comm1.Send_Data(" ");
		// Average of Data
		comm1.Send_Data(average());
		comm1.Send_Data(" ");
		// Length of Data
		comm1.Send_Data(LENGTH);
		comm1.Send_Data(" ");
		// Unique Stop Character
		comm1.Send_Data(0xBEEF);
		comm1.Send_Data(" ");

		usleep(100000);
	}
}

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


