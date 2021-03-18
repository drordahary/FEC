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
    int packetSize = 0;

    deserializer.deserializePacket(this->buffer);

    channelID = deserializer.getChannelID();
    fileID = deserializer.getFileID();
    packetID = deserializer.getPacketID();
    packetSize = strnlen(this->buffer, this->bufferSize);

    if (!redisHandler.fileExists(channelID, fileID)) // File did not receive meta data
    {
        fileTracker.handleUntrackedFile(channelID, fileID, packetID, this->buffer);

        if (this->handlingThreads.find(fileID) == this->handlingThreads.end())
        {
            this->handlingThreads.insert({fileID, false});
            std::thread(&RXDataReceiver::checkUntrackedFile, this, channelID, fileID, packetID, packetSize).detach();
        }
    }

    else
    {
        if (fileTracker.isFileUntracked(fileID)) // Untracked file
        {
            if (this->handlingThreads.find(fileID) == this->handlingThreads.end() || !this->handlingThreads.at(fileID))
            {
                this->receivedFiles.insert({fileID, fileTracker.getUntrackedPackets(fileID)});
                slog_trace("tracking file from main function");
                fileTracker.trackFile(channelID, fileID, this->redisHandler.getFileName(fileID, channelID),
                                      packetSize, this->redisHandler.getFileSize(channelID, fileID, packetSize));
            }
        }

        if (this->handlingThreads.find(fileID) == this->handlingThreads.end())
        {
            handleData(channelID, fileID, packetID, packetSize);
        }

        else
        {
            this->handlingThreads.erase(fileID);
        }
    }
}

void RXDataReceiver::handleData(int channelID, int fileID, int packetID, int packetSize)
{
    /* and will write the 
	   received buffer to a matched file
       If the function receiving its first buffer 
       it'll create a new file to work on 
       If the current file ID has been changed it'll close
       the current file and will open a new one */

    std::lock_guard<std::mutex> guard(this->mLock);
    int fileSize = 0;
    std::string fileName;

    fileSize = this->redisHandler.getFileSize(channelID, fileID, packetSize);

    if (this->receivedFiles.find(fileID) == this->receivedFiles.end())
    {
        this->receivedFiles.insert(std::pair<int, std::vector<int>>(fileID, std::vector<int>()));
    }

    auto start = this->receivedFiles.at(fileID).begin();
    auto stop = this->receivedFiles.at(fileID).end();

    if (std::find(start, stop, packetID) == stop)
    {
        this->receivedFiles.at(fileID).push_back(packetID);
        slog_trace("calling getFileName()...");
        fileName = this->redisHandler.getFileName(fileID, channelID);
        slog_trace("in handleData() file name is: %s", fileName.c_str());

        int packetSize = strnlen(this->buffer, this->bufferSize);

        this->fileBuilder.setFile(std::string(FILES_PATH) + "/" + fileName, 'w');
        this->fileBuilder.writeToFile(this->buffer, packetSize, fileTracker.fileMonitor.calculateOffset(fileSize, packetID, packetSize));
        this->fileBuilder.closeFile();
    }
}

void RXDataReceiver::checkUntrackedFile(int channelID, int fileID, int packetID, int packetSize)
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
                slog_trace("tracking file from thread");
                this->fileTracker.trackFile(channelID, fileID, this->redisHandler.getFileName(fileID, channelID),
                                            packetSize, this->redisHandler.getFileSize(channelID, fileID, packetSize));
                handleData(channelID, fileID, packetID, packetSize);
            }

            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    this->handlingThreads.erase(fileID);
}