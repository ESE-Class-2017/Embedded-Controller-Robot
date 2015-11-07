/*
Purpose: Weather Station Module
	 Serial Communication for Beaglebone
	 Class Definition
 Author: Josh Osborne
Version: 00
*/

#ifndef _COMM_H
#define _COMM_H

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include "queue.h"

#define BAUD_RATE B9600
#define SERIAL_PORT "/dev/ttyO4"

class Serial_Comm
{
	public:
		Serial_Comm();
		//~Serial_Comm();
		void Open_Port();
		void Initialize_Port();
		void Close_Port();
		bool Write_Port(std::string data);

	private:
		int fd;

};

#endif
