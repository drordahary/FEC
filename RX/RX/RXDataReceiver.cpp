#include "RXDataReceiver.h"

RXDataReceiver::RXDataReceiver(unsigned int port, std::string workingChannel) : RXReceiver(port, workingChannel),
                                                                                redisHandler(1)
{
    /* The constructor will first call the base class 
       constructor in order to initialize the socket,
       then the rest of the fields */

    this->deserializer = Deserializer();
    this->currentFileID = -1;
    this->currentChannelID = -1;

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
    /* This function will listen 
       to incoming UDP packets */

    while (true)
    {
        std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

        receivePacket();
        handleData();
    }
}

void RXDataReceiver::handleData()
{
    /* and will write the 
	   received buffer to a matched file
       If the function receiving its first buffer 
       it'll create a new file to work on 
       If the current file ID has been changed it'll close
       the current file and will open a new one */

    int channelID = 0;
    int fileID = 0;
    int packetID = 0;

    deserializer.deserializePacket(this->buffer);

    channelID = deserializer.getChannelID();
    std::cout << channelID << std::endl;
    fileID = deserializer.getFileID();
    packetID = deserializer.getPacketID();

    if (channelID != currentChannelID || fileID != currentFileID)
    {
        handlePacket(fileID, channelID);
        channelID = currentChannelID;
        fileID = currentFileID;
    }

    this->fileBuilder.writeToFile(this->buffer);
}

void RXDataReceiver::handlePacket(int fileID, int channelID)
{
    /* This function will  */

    std::string fileName = this->redisHandler.getFileName(fileID, channelID);
    int pos = fileName.find(':');

    if (pos != std::string::npos)
    {
        fileName = fileName.substr(0, pos);
        this->fileBuilder.closeFile();
        this->fileBuilder.setFile(std::string(FILES_PATH) + "/" + fileName);
    }
}