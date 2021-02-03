#pragma once

#include "TXSender.h"
#include "ThreadPool.h"

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
    ThreadPool *pool;
    FileMetaData *metaData;
    std::vector<std::string> paths;
    int channelID;
    int lastUpdatedFileID;
    std::map<std::string, std::string> fields;

    void saveToRedis();
    void bufferBuilder();

public:
    TXMetaDataSender(std::string IP, unsigned int port, std::string workingChannel, int channelID, int bufferSize, ThreadPool *pool);
    ~TXMetaDataSender();

    void sendMetaData();
};