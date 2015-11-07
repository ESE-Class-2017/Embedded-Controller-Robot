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

#define BAUD_RATE B9600
#define SERIAL_PORT "/dev/ttyO4"

class Serial_Comm
{
	public:
		Serial_Comm();
		//~Serial_Comm();
		void Open_Port();
		void Initialize_Port();
		void Send_Data(std::string data);
		void Close_Port();
		void Write_Port(std::string data);
	private:
		send_queue queue;
		bool port_status;
		int fd;
		std::thread serial;
};

#endif
