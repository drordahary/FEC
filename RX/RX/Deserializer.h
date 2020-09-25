#pragma once

#include <string.h>
#include <string>

#define HEX_LENGTH 10
#define BUFFER_SIZE 512
#define BASE_16 16

class Deserializer
{
private:
	char deserializedBuffer[BUFFER_SIZE - HEX_LENGTH + 1];

public:
	Deserializer();
	~Deserializer();

	int deserializePacket(char buffer[]);
	int hexToInt(std::string hexadecimal);
};
