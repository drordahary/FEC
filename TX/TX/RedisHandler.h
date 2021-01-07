#pragma once

#include <iostream>
#include <hiredis/hiredis.h>
#include <string.h>
#include <mutex>

class RedisHandler
{
private:
    redisContext *context;
    redisReply *reply;
    int databaseID;

    void checkExecution();
    std::string formatCommand(std::string fileMetaData[], int fileID);

public:
    static std::mutex mLock;

    RedisHandler(int databaseID);
    ~RedisHandler();

    void connectToRedis();
    int addToRedis(std::string fileMetaData[]);

    int getDirectoryCount();
    std::string getChannelName(int channelID);

    int getLastFileID();
    std::string getFileName(int fileID);

    std::string getValue(std::string key);

    void closeConnection();
};