#include "Serializer.h"

Serializer::Serializer()
{
	/* The constructor will inizialize the variable
	   packetCount which used for numbering the packets */

	this->packetCount = 0;
	std::fill(this->serializedBuffer, this->serializedBuffer + (BUFFER_SIZE + 1), '\0');
}

Serializer::~Serializer()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void Serializer::serializePacket(char buffer[], int fileID, int channelID)
{
	/* This function will serialize the buffer so the 
	   first 30 characters are a hexadecimals values
	   and the next 502 characters are the actual message */

	// NOTE: C - Channel, P - Packet, F - File

	std::string hexadecimalP = intToHex(this->packetCount);
	std::string hexadecimalF = intToHex(fileID);
	std::string hexadecimalC = intToHex(channelID);

	std::copy(hexadecimalC.c_str(), hexadecimalC.c_str() + HEX_LENGTH, this->serializedBuffer);
	strncat(this->serializedBuffer, hexadecimalF.c_str(), HEX_LENGTH);
	strncat(this->serializedBuffer, hexadecimalP.c_str(), HEX_LENGTH);

	strncat(this->serializedBuffer, buffer, BUFFER_SIZE + 1 - (HEX_LENGTH * 3));
	std::copy(this->serializedBuffer, this->serializedBuffer + BUFFER_SIZE + 1, buffer);
	std::cout << this->serializedBuffer << std::endl;

	std::fill(this->serializedBuffer, this->serializedBuffer + BUFFER_SIZE + 1, '\0');
	this->packetCount++;
}

std::string Serializer::intToHex(int ID)
{
	/* This function will parse the field 
	   packetCount to a hexadecimal value */

	std::stringstream hexadecimal;

	hexadecimal << "0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << ID;
	return hexadecimal.str();
}