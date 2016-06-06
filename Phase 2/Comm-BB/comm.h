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

#define LENGTH 100

//#define BAUD_RATE B9600


//#define SERIAL_PORT "/dev/ttyO4"

class Serial_Comm
{
	public:
		Serial_Comm();
		//~Serial_Comm();
		void Open_Port(char* port);
		void Initialize_Port(int BAUD_RATE);
		void Close_Port();
		bool Write_Port(std::string data);
		void Read_Port();
		bool Read_Empty();
		std::string Read_Data();
		void Send_Data(std::string data);
		void Send_Data(unsigned int data);
		void Send_Packet();
		void Read_Packet();
	private:
		Queue send_queue;
		Queue read_queue;
		bool port_status;
		int fd;
		std::thread serial_write;
		std::thread serial_read;
};

#endif
