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
	Serial_Comm comm1;

	comm1.Open_Port();
	comm1.Initialize_Port();

	for(int i = 0; i < 10000; i++)
	{
		comm1.Send_Data("josh");
		usleep(100);
	}
	comm1.Close_Port();
	return 0;
}
