#pragma once

#include <string.h>
#include <string>

#define HEX_LENGTH 10
#define BUFFER_SIZE 512
#define BASE_16 16

class Deserializer
{
private:
	char deserializedBuffer[BUFFER_SIZE - (HEX_LENGTH * 2) + 1];
	int channelID;
	int fileID;
	int packetID;

public:
	Deserializer();
	~Deserializer();

	void deserializePacket(char buffer[]);
	int hexToInt(std::string hexadecimal);

	int getChannelID();
	int getFileID();
	int getPacketID();
};
