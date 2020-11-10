#include "RXMetaDataReceiver.h"

RXMetaDataReceiver::RXMetaDataReceiver(unsigned int port) : RXReceiver(port),
                                                            redisHandler(1)
{
    /* The constructor will first call the base class to 
       initialize connection and then connect to Redis */

    this->fileMetaData = new FileMetaData();
}

RXMetaDataReceiver::~RXMetaDataReceiver()
{
    /* The destructor will close the socket manually
       and the rest allocated memory automatically */

    this->redisHandler.closeConnection();
    delete this->fileMetaData;
}

void RXMetaDataReceiver::receiveMetaData()
{
    /* This function will receive the meta data
       and will save that data to Redis */

    while (true)
    {
        std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), 0);

        receivePacket();
    }
}

void RXMetaDataReceiver::organizeData()
{
    /* The function will split the metaData
       and will also set the struct FileMetaData */

    std::string metaData = "";

    while (true)
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        this->queueCondition.wait(lock); // Waiting for queue to fill

        metaData = this->packetsReceived.front();
        this->packetsReceived.pop();

        lock.unlock();

        char delimeter = ',';
        size_t position = 0;

        int i = 0;

        while (((position = metaData.find(delimeter)) != std::string::npos) && i < 2)
        {
            this->splittedData[i] = metaData.substr(0, position);
            metaData.erase(0, position + 1);

            i++;
        }

        this->splittedData[2] = metaData;

        this->fileMetaData->ID = std::stoi(this->splittedData[0]);
        this->fileMetaData->filename = this->splittedData[1];
        this->fileMetaData->size = std::stoi(this->splittedData[2]);

        createStructure(FILES_PATH, this->fileMetaData->filename);
        saveToRedis();
    }
}

void RXMetaDataReceiver::saveToRedis()
{
    /* This fucntion will handle the received
       data and will send it to Redis */

    std::string parameters[2];

    parameters[0] = this->fileMetaData->filename;
    parameters[1] = std::to_string(this->fileMetaData->size);

    this->redisHandler.addToRedis(parameters);
}

void RXMetaDataReceiver::startReceiving()
{
    /* This function will initialize the threads
       that receive and handle the data */

    std::thread receiverThread(&RXMetaDataReceiver::receiveMetaData, this);
    std::thread organizerThread(&RXMetaDataReceiver::organizeData, this);

    receiverThread.join();
    organizerThread.join();
}