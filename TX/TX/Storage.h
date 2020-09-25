#pragma once

#include <iostream>
#include <string>

#define BURST 4
#define BUFFER_SIZE 512

class Storage
{
private:
	std::string* packets;
	int currentAdded;

public:
	Storage();
	~Storage();

	void addToStorage(char buffer[]);
	void clearStorage();

	std::string* getStorage();
};