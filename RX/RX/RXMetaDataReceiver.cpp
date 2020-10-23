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

    delete this->fileMetaData;
    close(this->sc);
}

void RXMetaDataReceiver::receiveMetaData()
{
    /* This function will receive the meta data
       and will save that data to Redis */

    while (true)
    {
        std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

        receivePacket();

        std::string content = this->buffer;
        organizeData(content);

        saveToRedis();
    }
}

void RXMetaDataReceiver::organizeData(std::string metaData)
{
    /* The function will split the metaData
       and will also set the struct FileMetaData */

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