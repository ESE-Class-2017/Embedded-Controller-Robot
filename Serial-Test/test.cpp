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

int main()
{
	Serial_Comm comm1;

	comm1.Open_Port();
	comm1.Initialize_Port();

	for(int i = 0; i < 10; i++)
		comm1.Write_Port("josh");

	comm1.Close_Port();
	return 0;
}
