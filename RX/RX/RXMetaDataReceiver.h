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
    std::string splittedData[3];
    FileMetaData *fileMetaData;

    RedisHandler redisHandler;
    void saveToRedis();

    void setMetaData();

public:
    RXMetaDataReceiver(unsigned int port);
    ~RXMetaDataReceiver();

    void receiveMetaData();
    void organizeData(std::string metaData);
};