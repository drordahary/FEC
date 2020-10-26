#pragma once

#include <iostream>
#include <hiredis/hiredis.h>
#include <string.h>

class RedisHandler
{
private:
    redisContext* context;
    redisReply* reply;
    int databaseID;

    void checkExecution();
    std::string formatCommand(std::string fileMetaData[], int fileID);

public:
    RedisHandler(int databaseID);
    ~RedisHandler();

    void connectToRedis();
    int addToRedis(std::string fileMetaData[]);
    int getLastFileID();
    void closeConnection();
};