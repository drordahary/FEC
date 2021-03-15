#include "RXDataReceiver.h"

RXDataReceiver::RXDataReceiver(unsigned int port, std::string workingChannel, int bufferSize) : RXReceiver(port, workingChannel, bufferSize),
                                                                                                redisHandler(1),
                                                                                                deserializer(bufferSize),
                                                                                                fileTracker(bufferSize)
{
    /* The constructor will first call the base class 
       constructor in order to initialize the socket,
       then the rest of the fields */

    this->lastUpdatedPacketID = -1;

    std::fill(this->buffer, this->buffer + (bufferSize + 1), '\0');
}

RXDataReceiver::~RXDataReceiver()
{
    /* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

    this->fileBuilder.closeFile();
}

void RXDataReceiver::receiveData()
{
    /* This function will listen 
       to incoming UDP packets */

    int receiveCompleted = 1;

    while (receiveCompleted == 1)
    {
        std::fill(this->buffer, this->buffer + (this->bufferSize + 1), '\0');

        receiveCompleted = receivePacket();
        if (receiveCompleted == 1)
        {
            assumeCase();
        }
    }
}

void RXDataReceiver::assumeCase()
{
    /* This function will check to see if the case is 
       about an existing file or about an untracked file */

    int channelID = 0;
    int fileID = 0;
    int packetID = 0;

    deserializer.deserializePacket(this->buffer);

    channelID = deserializer.getChannelID();
    fileID = deserializer.getFileID();
    packetID = deserializer.getPacketID();

    if (!redisHandler.fileExists(channelID, fileID)) // File did not receive meta data
    {
        fileTracker.handleUntrackedFile(channelID, fileID, packetID, this->buffer);
        this->handlingThreads.insert({fileID, false});
        std::thread(&RXDataReceiver::checkUntrackedFile, this, channelID, fileID).detach();
    }

    else
    {
        if (fileTracker.isFileUntracked(fileID)) // Untracked file
        {
            if (!this->handlingThreads.at(fileID))
            {
                this->receivedFiles.insert({fileID, fileTracker.getUntrackedPackets(fileID)});
                fileTracker.eraseUntrackedFile(fileID);

                fileTracker.trackFile(channelID, fileID, this->redisHandler.getFileName(fileID, channelID),
                                      strnlen(this->buffer, this->bufferSize));
            }
        }

        handleData(channelID, fileID, packetID);
    }
}

void RXDataReceiver::handleData(int channelID, int fileID, int packetID)
{
    /* and will write the 
	   received buffer to a matched file
       If the function receiving its first buffer 
       it'll create a new file to work on 
       If the current file ID has been changed it'll close
       the current file and will open a new one */

    int fileSize = 0;
    std::string fileName;

    fileSize = this->redisHandler.getFileSize(channelID, fileID);

    if (this->receivedFiles.find(fileID) == this->receivedFiles.end())
    {
        this->receivedFiles.insert(std::pair<int, std::vector<int>>(fileID, std::vector<int>()));
    }

    auto start = this->receivedFiles.at(fileID).begin();
    auto stop = this->receivedFiles.at(fileID).end();

    if (std::find(start, stop, packetID) == stop)
    {
        this->receivedFiles.at(fileID).push_back(packetID);
        fileName = this->redisHandler.getFileName(fileID, channelID);

        int packetSize = strnlen(this->buffer, this->bufferSize);

        this->fileBuilder.setFile(std::string(FILES_PATH) + "/" + fileName, 'w');
        this->fileBuilder.writeToFile(this->buffer, packetSize, fileTracker.fileMonitor.calculateOffset(fileSize, packetID, packetSize));
        this->fileBuilder.closeFile();
    }
}

void RXDataReceiver::checkUntrackedFile(int channelID, int fileID)
{
    /* This function will check every 100ms if for 5 times
       the file received its meta data in Redis
       Will be mainly used for the case where all of the
       packets were already received so it will keep checking
       and won't be on a blocking socket */

    for (int i = 0; i < 5; i++)
    {
        if (this->redisHandler.fileExists(channelID, fileID))
        {
            if (this->receivedFiles.find(fileID) == this->receivedFiles.end())
            {
                this->handlingThreads[fileID] = true;

                this->receivedFiles.insert({fileID, fileTracker.getUntrackedPackets(fileID)});
                fileTracker.eraseUntrackedFile(fileID);
                this->fileTracker.trackFile(channelID, fileID, this->redisHandler.getFileName(fileID, channelID),
                                            strnlen(this->buffer, bufferSize));
                
                this->handlingThreads[fileID] = false;
            }

            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}