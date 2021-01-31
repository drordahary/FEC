#include "RXDataReceiver.h"

RXDataReceiver::RXDataReceiver(unsigned int port, std::string workingChannel, int bufferSize) : RXReceiver(port, workingChannel, bufferSize),
                                                                                                redisHandler(1),
                                                                                                deserializer(bufferSize)
{
    /* The constructor will first call the base class 
       constructor in order to initialize the socket,
       then the rest of the fields */

    this->currentFileID = -1;

    std::fill(this->buffer, this->buffer + (bufferSize + 1), '\0');
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
        std::fill(this->buffer, this->buffer + (this->bufferSize + 1), '\0');

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
    int fileSize = 0;
    std::string fileName;

    deserializer.deserializePacket(this->buffer);

    channelID = deserializer.getChannelID();
    fileID = deserializer.getFileID();
    packetID = deserializer.getPacketID();

    fileSize = this->redisHandler.getFileSize(channelID, fileID);

    if (this->receivedFiles.find(fileID) == this->receivedFiles.end())
    {
        this->receivedFiles.insert(std::pair<int, std::vector<int>>(fileID, std::vector<int>()));
        this->fullyReceivedFiles.insert(std::pair<int, bool>(fileID, false));
    }

    auto start = this->receivedFiles.at(fileID).begin();
    auto stop = this->receivedFiles.at(fileID).end();

    if ((std::find(start, stop, packetID) == stop) && !this->fullyReceivedFiles.at(fileID))
    {
        this->receivedFiles.at(fileID).push_back(packetID);

        if (fileID != currentFileID)
        {
            fileName = handlePacket(fileID, channelID);
            fileID = currentFileID;
        }

        int packetSize = strnlen(this->buffer, this->bufferSize);

        this->fileBuilder.setFile(std::string(FILES_PATH) + "/" + fileName);
        this->fileBuilder.writeToFile(this->buffer, packetSize, calculateOffset(fileSize, packetID, packetSize));
        this->fileBuilder.closeFile();
    }
}

int RXDataReceiver::calculateOffset(int fileSize, int packetID, int packetSize)
{
    /* The function will calculate the offset 
       which we need to start writing from */

    int fullBuffersCount = fileSize / (this->bufferSize - (HEX_LENGTH * 3));
    int offset = 0;

    if (packetSize < (this->bufferSize - (HEX_LENGTH * 3)))
    {
        offset = packetID * fullBuffersCount * (this->bufferSize - (HEX_LENGTH * 3));
    }

    else
    {
        offset = packetID * packetSize;
    }

    return offset;
}

std::string RXDataReceiver::handlePacket(int fileID, int channelID)
{
    /* This function will  */

    std::string fileName = this->redisHandler.getFileName(fileID, channelID);
    int pos = fileName.find(':');

    if (pos != std::string::npos)
    {
        fileName = fileName.substr(0, pos);
    }

    return fileName;
}

void RXDataReceiver::checkMaxSize(int fileID, int fileSize)
{
    /* This function will check to see if a certain file
       has already received the maximum amount 
       of bytes to remove it from the map */

    int currentTotalSize = 0;

    for (auto &packet : this->receivedFiles.at(fileID))
    {
        currentTotalSize += this->bufferSize;
    }

    if (currentTotalSize >= fileSize)
    {
        this->receivedFiles.erase(fileID);
    }

    this->fullyReceivedFiles.at(fileID) = true;
}