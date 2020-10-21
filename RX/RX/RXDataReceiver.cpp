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
    close(sc);
}

void RXDataReceiver::receivePackets()
{
    /* This function will listen to incoming
	   UDP packets and will write the 
	   received buffer to a file */

    this->fileBuilder.setFile(FILES_PATH);

    int packetNumber = 0;

	while (true)
	{
		std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

		if (recv_len = recvfrom(sc, this->buffer, BUFFER_SIZE + 1, 0, (struct sockaddr*) &cliaddr, &slen) < 0)
		{
			std::cout << "Failed to receive" << std::endl;
			exit(EXIT_FAILURE);
		}

		packetNumber = deserializer.deserializePacket(this->buffer);
		
		this->fileBuilder.writeToFile(this->buffer);
	}
}