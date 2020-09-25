#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>

#define HEX_LENGTH 10
#define BUFFER_SIZE 512

class Serializer
{
private:
	char serializedBuffer[BUFFER_SIZE + 1];
	int packetCount;

public:
	Serializer();
	~Serializer();

	void serializePacket(char buffer[]);
	std::string intToHex();
};