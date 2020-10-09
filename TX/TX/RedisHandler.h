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

public:
    RedisHandler(int databaseID);
    ~RedisHandler();

    void connectToRedis();
    void addToRedis(std::string fileMetaData);
    void getFromRedis(int fileID);
};