#pragma once

#include "TXSender.h"

typedef struct FileMetaData
{
    unsigned int ID;
    std::string filePath;
    unsigned int size;

} FileMetaData;

class TXMetaDataSender : public TXSender
{
private:
    DirectoryReader directoryReader;
    FileReader fileReader;
    RedisHandler redisHandler;
    FileMetaData *metaData;
    std::vector<std::string> paths;
    int channelID;
    std::map<std::string, std::string> fields;

    void saveToRedis();
    void bufferBuilder();

public:
    TXMetaDataSender(std::string IP, unsigned int port, std::string workingChannel, int channelID, int bufferSize);
    ~TXMetaDataSender();

    void sendMetaData();
};