/*
Purpose: Class definitions for queue 
 Author: Josh Osborne
*/

#include "queue.h"
#include <iostream>

typedef std::lock_guard<std::mutex> MutexLock;

// Constructor
Queue::Queue()
{}

// check to see if queue is empty
bool Queue::empty()
{
	return queue.empty();
}

// Push packet onto queue
void Queue::push(const std::string& packet)
{
	MutexLock lock(mutex);
	
	queue.push(packet);
}

// pop the string that will be written to serial
std::string Queue::pop()
{
	MutexLock lock(mutex);
	
	std::string temp = queue.front(); //get string to be sent
	queue.pop(); 					  // delete string from queue
	
	return temp;
}


