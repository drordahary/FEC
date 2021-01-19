#pragma once

#include "RXReceiver.h"

typedef struct FileMetaData
{
    unsigned int ID;
    std::string filename;
    unsigned int size;

} FileMetaData;

class RXMetaDataReceiver : public RXReceiver
{
private:
    std::string splittedData[4];
    FileMetaData *fileMetaData;
    std::map<std::string, std::string> fields;
    int channelID;

    RedisHandler redisHandler;
    void saveToRedis();

    void setMetaData();

public:
    RXMetaDataReceiver(unsigned int port, std::string workingChannel);
    ~RXMetaDataReceiver();

    void receiveMetaData();
    void organizeData(std::string metaData);
};