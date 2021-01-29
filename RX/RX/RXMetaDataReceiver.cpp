#include "RXMetaDataReceiver.h"

RXMetaDataReceiver::RXMetaDataReceiver(unsigned int port, std::string workingChannel, int bufferSize) : RXReceiver(port, workingChannel, bufferSize),
                                                                                                        redisHandler(1)
{
    /* The constructor will first call the base class to 
       initialize connection and then connect to Redis */

    this->fileMetaData = new FileMetaData();
    this->channelIDAdded = false;
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
        std::fill(this->buffer, this->buffer + (this->bufferSize + 1), 0);

        receivePacket();
        organizeData(this->buffer);
    }
}

void RXMetaDataReceiver::organizeData(std::string metaData)
{
    /* The function will split the metaData
       and will also set the struct FileMetaData */

    char delimeter = ',';
    size_t position = 0;

    int i = 0;

    while (((position = metaData.find(delimeter)) != std::string::npos) && i < 3)
    {
        this->splittedData[i] = metaData.substr(0, position);
        metaData.erase(0, position + 1);

        i++;
    }

    this->splittedData[3] = metaData;
    setMetaData();

    createStructure(FILES_PATH, this->fileMetaData->filename);
    this->fileBuilder.createNewFile(this->fileMetaData->size, std::string(FILES_PATH) + "/" +
                                                                  this->fileMetaData->filename);

    saveToRedis();
    runTime++;
}

void RXMetaDataReceiver::setMetaData()
{
    /* The function will set the 
       struct of the meta data */

    this->channelID = std::stoi(this->splittedData[0]);
    this->fileMetaData->ID = std::stoi(this->splittedData[1]);
    this->fileMetaData->filename = this->splittedData[2];
    this->fileMetaData->size = std::stoi(this->splittedData[3]);

    this->fields.insert({"fileID:" + std::to_string(this->fileMetaData->ID),
                         this->fileMetaData->filename + ":" + std::to_string(this->fileMetaData->size)});
}

void RXMetaDataReceiver::saveToRedis()
{
    /* This fucntion will handle the received
       data and will send it to Redis */

    std::string key = "channelID:" + std::to_string(this->channelID);
    this->redisHandler.addMetaData(this->fields, key, this->channelIDAdded);
    this->fields.clear();
    channelIDAdded = true;
}