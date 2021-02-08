#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>

#define HEX_LENGTH 10

class Serializer
{
private:
	char *serializedBuffer;
	int packetCount;
	int bufferSize;

public:
	Serializer(int bufferSize);
	~Serializer();

	void resetPacketCount();

	int serializePacket(char buffer[], int fileID, int channelID);
	std::string intToHex(int ID);
};