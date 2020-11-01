#pragma once

#include "TXSender.h"

typedef struct FileMetaData
{
    unsigned int ID;
    std::string filename;
    unsigned int size;

} FileMetaData;

class TXMetaDataSender : public TXSender
{
private:
    DirectoryReader directoryReader;
    FileReader fileReader;
    RedisHandler redisHandler;
    FileMetaData *metaData;

    int saveToRedis();
    void bufferBuilder();

public:
    TXMetaDataSender(std::string IP, unsigned int port);
    ~TXMetaDataSender();

    void sendMetaData();
};