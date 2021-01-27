#pragma once

#include <string.h>
#include <string>

#define HEX_LENGTH 10
#define BASE_16 16

class Deserializer
{
private:
	char *deserializedBuffer;
	int channelID;
	int fileID;
	int packetID;
	int bufferSize;

public:
	Deserializer(int bufferSize);
	~Deserializer();

	void deserializePacket(char buffer[]);
	int hexToInt(std::string hexadecimal);

	int getChannelID();
	int getFileID();
	int getPacketID();
};
