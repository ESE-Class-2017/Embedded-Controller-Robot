/*
Purpose: Class for Queue of Serial Transmission Data
 Author: Josh Osborne
*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>

class Queue
{
	public:
		Queue();
		bool empty();
		void push(const std :: string& packet);
		std::string pop();
	
	private:
		std::queue<std::string> queue; // for using c++ queue functions
		std::mutex mutex;   			   // for safe accessing the queue
	
};

#endif
