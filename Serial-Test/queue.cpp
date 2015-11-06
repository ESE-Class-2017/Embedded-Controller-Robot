/*
Purpose: Class definitions for queue 
 Author: Josh Osborne
*/

// Constructor
Queue::Queue()
{}

// check to see if queue is empth
bool Queue::empty()
{
	return queue.empty();
}

// Push packet onto queue
void Queue::push(const std :: string& packet)
{
	// Add mutex lock
	queue.push(packet);
}

// pop the string that will be written to serial
std::string pop()
{
	// Add mutex lock
	
	std::string temp = queue.front(); //get string to be sent
	queue.pop(); 					  // delete string from queue
	
	return temp;
}


