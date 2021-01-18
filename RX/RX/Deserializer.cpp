#include "Deserializer.h"

Deserializer::Deserializer()
{
	/* The constructor will not do anything 
	   other than creating the object */
}

Deserializer::~Deserializer()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
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

	std::copy(buffer + (HEX_LENGTH * 3), buffer + BUFFER_SIZE + 1, this->deserializedBuffer);

	std::fill(buffer, buffer + BUFFER_SIZE + 1, '\0');
	std::copy(this->deserializedBuffer, this->deserializedBuffer + BUFFER_SIZE - (HEX_LENGTH * 3) + 1, buffer);

	std::fill(this->deserializedBuffer, this->deserializedBuffer + BUFFER_SIZE - (HEX_LENGTH * 3) + 1, '\0');
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