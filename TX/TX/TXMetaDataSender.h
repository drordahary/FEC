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

    int saveToRedis();
    void bufferBuilder();

    void createStructure(std::string newPath, std::string path);
    void moveFiles(std::string newPath);

public:
    TXMetaDataSender(std::string IP, unsigned int port);
    ~TXMetaDataSender();

    void sendMetaData();
};

void createFolder(std::string left, std::string currentPath);