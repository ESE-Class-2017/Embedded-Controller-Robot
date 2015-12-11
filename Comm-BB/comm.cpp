/*
Purpose: Weather Station Module
	 Serial Communication for BeagleBone
	 Class Functions
 Author: Josh Osborne
Version: 00
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
#include <thread>
#include "comm.h"
#include "queue.h"

Serial_Comm::Serial_Comm()
:port_status(false)
{}

void Serial_Comm::Open_Port(char* port)
{
	/* Open the port
	O_RDWR   - open for read and write
	O_NOCTTY - dont make controlling terminal
	O_NDELAY - ignore state of DCD line	
	*/
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	// Failed to open port
	if(fd == -1)
	{
		std::cout << "open_port: unable to open port " << port << std::endl;
		exit(1);
	}
	else fcntl(fd, F_SETFL, FNDELAY);
}

void Serial_Comm::Initialize_Port(int BAUD_RATE)
{
	struct termios options;
	
	// Get the current options for the port
	if (tcgetattr(fd, &options) < 0)
		perror("Failed to load current port config:");

	// Set tth baud rate
	cfsetispeed(&options, BAUD_RATE);
	cfsetospeed(&options, BAUD_RATE);
	
	// Enable reviecer and set local mode
	options.c_cflag |= (CLOCAL | CREAD);

	// Set Parity checking
	// No parity, 8 bits, 1 stop bit 
	options.c_cflag &= ~PARENB; // Set no parity
	options.c_cflag &= ~CSTOPB; // Set one stop bit
	options.c_cflag &= ~CSIZE; // Mask the character size bits
	options.c_cflag |= CS8;	   // Select 8 data bits
	
	// Turn hardware flow control off
	options.c_cflag &= ~CRTSCTS;
	
	
	// Select non-connical output
	// -ICANON	Enable canonical input (else raw)
	// -ECHO	Enable echoing of input characters
	// -ECHOE	Echo erase character as BS-SP-BS
	// -ISIG	Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// To select connical output
	//options.c_lflag |= (ICANON | ECHO | ECHOE);
	
	// Set read input parameters (using select() instead)
	// -VMIN	number of minimum input characters
	// -VTIME	input buffer read time
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 30;

	// Set input parity (No input parity)
	// -INPCK	Enable parity check
	// -ISTRIP	Strip parity bits
	// -IGNPAR	Ignore parity errors
	// -PARMRK	Mark parity errors
	// options.c_iflag |= (INPCK | ISTRIP);

	// Disable Software flow control
	// -IXON	Enable software flow control (outgoing)
	// -IXOFF	Enable software flow control (incoming)
	// -IXANY	Allow any character to start flow again
	options.c_iflag &= ~(IXON | IXOFF | IXANY);

	// Set processed output
	//options.c_oflag |= OPOST;
	// Set raw output
	options.c_oflag &= ~OPOST;

	// Set new options for the port
	// -TCSANOW	Make changes now without waiting for data to complete
	// -TCSADRAIN	Wait until everything has been transmitted
	// -TCSAFLUSH	Flush input and output buffers and make the change
	//tcsetattr(fd, TCSANOW, &options);
	tcsetattr(fd, TCSAFLUSH, &options);
	
	//sleep(2); // some people say its nessesary
	// Flush the output buffer
 	tcflush(fd,TCIOFLUSH); 
	
	port_status = true;
	
	serial_write = std::thread(&Serial_Comm::Send_Packet, this);
	serial_read = std::thread(&Serial_Comm::Read_Packet, this);
	
	
}

void Serial_Comm::Close_Port()
{
	port_status = false;
	
	//terminate thread
	serial_write.join();
	serial_read.join();

	// Close Port
	close(fd);
}


void Serial_Comm::Send_Packet()
{
	while(port_status)
	{
		while(!send_queue.empty())
		{
			std::string packet(send_queue.pop());
			//std::cout << "Send_Packet: " << packet << std::endl;
			Write_Port(packet);
		}
	}
}

void Serial_Comm::Read_Packet()
{
	while(port_status)
	{	
			Read_Port();
	}
}

std::string Serial_Comm::Read_Data()
{
	return read_queue.pop();
}

bool Serial_Comm::Read_Empty()
{
	return read_queue.empty();
}

bool Serial_Comm::Write_Port(std::string data)
{
	int num;  // Number of characters written to port
	const char *c = data.c_str();
	
		num = write(fd, c, data.length());
		//std::cout << num << std::endl;
		
		// check for write error
		if(num < 0)
		{
			//std::cout << "Write_Port: Write failed" << std::endl;
			return false;
		}
		
		//usleep(100); // Does Natesh Think I need this
		return true;
}

void Serial_Comm::Send_Data(std::string data)
{
	send_queue.push(data);
}

void Serial_Comm::Send_Data(unsigned int data)
{
	send_queue.push(std::to_string(data));
}

void Serial_Comm::Read_Port()
{
	//std::cout << "Read_Port()" << std::endl;
		
	int i;
	//char *buf = new char[LENGTH];
	char buf[LENGTH];
	//std::string buf;
	int retval;
	struct timeval tv;	// Time parameter structure for select() 
	fd_set rfds, temp;	// File descripter flag for the select() function
	
	
	// Set up the input read delay
	// Clear the set
	// void FD_ZERO(fd_set *set);
	FD_ZERO(&rfds);

	// Add given file descriptor from set
	// void FD_SET(int fd, fd_set *set);
	FD_SET(fd, &rfds);
	
	// Use temp variable because flag needs to be reset each time through loop
	temp = rfds;

	// Wait for specified time seconds
	tv.tv_sec = 2;	// seconds
	tv.tv_usec = 0; // microseconds

	// Do the read
	// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
	retval = select((fd+1), &temp, NULL, NULL, &tv);
	//std::cout << "retval: " << retval << std::endl;
	// Select() failed
	if(retval == -1)
	{
		perror("select() failed.");
	}
	// Select() sees data on port
	else if(retval)
	{
		i = read(fd, buf, LENGTH);
		
		buf[i] = '\0';
		
		if(i == -1)
			perror("read() Failed"); 

		std::string packet(buf);
		std::cout << "read: " << packet << std::endl;
		//std::cout << "len: " << packet.length() << std::endl;
		read_queue.push(packet);
		
	}
	// Select() sees no data on port
	else
	{
		printf("No data on the port!\n");
	}
	//printf("buf: %s\n", buf);


	
//	delete[] buf;
	
}
