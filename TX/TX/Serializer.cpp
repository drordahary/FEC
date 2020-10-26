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

void Serializer::serializePacket(char buffer[], int fileID)
{
	/* This function will serialize the buffer so the 
	   first 10 characters are a hexadecimal value
	   and the next 502 characters are the actual message */

	// NOTE: P - Packet, F - File

	std::string hexadecimalP = intToHex(this->packetCount);
	std::string hexadecimalF = intToHex(fileID);

    std::copy(hexadecimalF.c_str(), hexadecimalF.c_str() + HEX_LENGTH, this->serializedBuffer);
	strncat(this->serializedBuffer, hexadecimalP.c_str(), HEX_LENGTH);
	
	strncat(this->serializedBuffer, buffer, BUFFER_SIZE + 1 - (HEX_LENGTH * 2));
	std::copy(this->serializedBuffer, this->serializedBuffer + BUFFER_SIZE + 1, buffer);

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