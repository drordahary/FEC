#include "RXDataReceiver.h"

RXDataReceiver::RXDataReceiver(unsigned int port) : RXReceiver(port)
{
    /* The constructor will first call the base class 
       constructor in order to initialize the socket,
       then the rest of the fields */

    this->deserializer = Deserializer();

    std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');
}

RXDataReceiver::~RXDataReceiver()
{
    /* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

    this->fileBuilder.closeFile();
}

void RXDataReceiver::receiveData()
{
    /* This function will listen to incoming
	   UDP packets and will write the 
	   received buffer to a file */

    this->fileBuilder.setFile(FILES_PATH);

    int packetNumber = 0;

	while (true)
	{
		std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

		receivePacket();

		packetNumber = deserializer.deserializePacket(this->buffer);
		
		this->fileBuilder.writeToFile(this->buffer);
	}
}

void RXDataReceiver::startReceiving()
{
    /* This function will start a thread
       on the receiving function */

    std::thread receiveThread(&RXDataReceiver::receiveData, this);
    receiveThread.join();
}