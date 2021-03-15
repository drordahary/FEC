#include "Deserializer.h"

Deserializer::Deserializer(int bufferSize)
{
	/* The constructor will initialize the 
	   fields and create the object */

	this->bufferSize = bufferSize;
	this->deserializedBuffer = new char[bufferSize - (HEX_LENGTH * 3) + 1];
}

Deserializer::~Deserializer()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */

	delete[] this->deserializedBuffer;
}

void Deserializer::deserializePacket(char buffer[])
{
	/* This function will deserialize the packet to 
	   two parts: the packet metadata and the actual data */

	std::string channelHexadecimal(buffer, buffer + HEX_LENGTH);
	std::string fileHexadecimal(buffer + HEX_LENGTH, buffer + (HEX_LENGTH * 2));
	std::string packetHexadecimal(buffer + (HEX_LENGTH * 2), buffer + (HEX_LENGTH * 3));

	this->channelID = hexToInt(channelHexadecimal);
	this->fileID = hexToInt(fileHexadecimal);
	this->packetID = hexToInt(packetHexadecimal);

	std::copy(buffer + (HEX_LENGTH * 3), buffer + this->bufferSize + 1, this->deserializedBuffer);

	std::fill(buffer, buffer + this->bufferSize + 1, '\0');
	std::copy(this->deserializedBuffer, this->deserializedBuffer + this->bufferSize - (HEX_LENGTH * 3) + 1, buffer);

	std::fill(this->deserializedBuffer, this->deserializedBuffer + this->bufferSize - (HEX_LENGTH * 3) + 1, '\0');
}

int Deserializer::hexToInt(std::string hexadecimal)
{
	/* The function will convert the hexadecimal string (base 16) 
	   to a normal decimal value and will return it */

	return std::stoul(hexadecimal, nullptr, BASE_16);
}

int Deserializer::getChannelID()
{
	/* The function will return the channel ID */

	return this->channelID;
}

int Deserializer::getFileID()
{
	/* The function will return the file ID */

	return this->fileID;
}

int Deserializer::getPacketID()
{
	/* The function will return the packet ID */

	return this->packetID;
}