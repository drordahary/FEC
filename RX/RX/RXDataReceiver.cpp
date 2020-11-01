#include "RXDataReceiver.h"

RXDataReceiver::RXDataReceiver(unsigned int port) : RXReceiver(port),
                                                    redisHandler(1)
{
    /* The constructor will first call the base class 
       constructor in order to initialize the socket,
       then the rest of the fields */

    this->deserializer = Deserializer();
    this->currentFileID = 0;

    std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');
}

RXDataReceiver::~RXDataReceiver()
{
    /* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

    this->redisHandler.closeConnection();
    this->fileBuilder.closeFile();
}

void RXDataReceiver::receiveData()
{
    /* This function will listen to incoming
	   UDP packets and will write the 
	   received buffer to a matched file
       If the function receiving its first buffer 
       it'll create a new file to work on 
       If the current file ID has been changed it'll close
       the current file and will open a new one */

    int fileID = 0;
    int packetID = 0;

    std::string fileName;

	while (true)
	{
		std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

		receivePacket();

		deserializer.deserializePacket(this->buffer);

        fileID = deserializer.getFileID();
        packetID = deserializer.getPacketID();

        if (currentFileID == 0)
        {
            fileName = this->redisHandler.getFileName(fileID);
            this->fileBuilder.setFile(FILES_PATH + fileName);

            currentFileID = fileID;
        }

        else if(currentFileID != fileID)
        {
            fileName = this->redisHandler.getFileName(fileID);

            this->fileBuilder.closeFile();
            this->fileBuilder.setFile(FILES_PATH + fileName);

            currentFileID = fileID;
        }
        
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