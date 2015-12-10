/*
Purpose: Test the serial port code
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
#include "comm.h"
#include "queue.h"

int main()
{
	int num;
	int i;
	int j;
	vector<int> data;
	Serial_Comm comm1;

	comm1.Open_Port();
	comm1.Initialize_Port();

	/* initialize random seed: */
	srand (time(NULL));

	j = 0;
	// Generate random numbers to represent analog data
	for(i = 0; i < 10; i++)
	{
		/* generate secret number between 1 and 4096: */
		num = rand() % 4096 + 1;
		data[j] = num;
		j++;		
	}
	
	
	for(int i = 0; i < 10000000; i++)
	{
		std::string s = std::to_string(i);
		comm1.Send_Data("\x55");
		usleep(1000000);
		//comm1.Send_Data("\xAA");
		//usleep(1000000);
	}

	//std::cout << comm1.Read_Data() << std::endl;
	sleep(5);
	comm1.Close_Port();
	return 0;
}
