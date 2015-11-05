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
#include "comm.h"

Serial_Comm::Serial_Comm():
{}

void Serial_Comm::Open_Port()
{
	/* Open the port
	O_RDWR   - open for read and write
	O_NOCTTY - dont make controlling terminal
	O_NDELAY - ignore state of DCD line	
	*/
	fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	// Failed to open port
	if(fd == -1)
	{
		cout << "open_port: unable to open port " << SERIAL_PORT << endl;
		exit(1);
	}
	else fcntl(fd, F_SETFL, FNDELAY);
}

void Serial_Comm::Initalize_Port()
{
	struct termios options;
	
	// Get the current options for the port
	if (tcgetattr(fd, &options) < 0)
		perror("Failed to load current port config:");

	// Set tth baud rate to 9600
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
	//options.c_cc[VMIN] = 0;
	//options.c_cc[VTIME] = 30;

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
}

void Serial_Comm::Close_Port()
{
	// Close Port
	close(Serial);
}

Void Serial_Comm::Write_Port(string data)
{
	int num;  // Number of characters written to port

	num = write(fd, data, data.length());
	
	if(num < 0)
		cout << "Write_Port: Write failed" << endl;
}
