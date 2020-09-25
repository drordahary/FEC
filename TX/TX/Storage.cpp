#include "Storage.h"

Storage::Storage()
{
	/* Constructor will initialize the 
	   storage and the packet counter */

	this->packets = new std::string[BURST];
	this->currentAdded = 0;
}

Storage::~Storage()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */

	delete[] this->packets;
}

void Storage::addToStorage(char buffer[])
{
	/* The function will receive a buffer (512 characters long)
	   and will add it to the storage */

	this->packets[this->currentAdded] = buffer;
	this->currentAdded++;
}

void Storage::clearStorage()
{
	/* The function will reset 
	   every field of the object */

	for (int i = 0; i < BURST; i++)
	{
		this->packets[i].clear();
	}

	this->currentAdded = 0;
	
}

std::string* Storage::getStorage()
{
	/* The function will return 
	   the packets in the storage */

	return this->packets;
}