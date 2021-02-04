#pragma once

#include <iostream>
#include <hiredis/hiredis.h>
#include <string.h>
#include <mutex>
#include <map>
#include <vector>

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

    void addMetaData(std::map<std::string, std::string> &fields, std::string &key,
                     bool channelIDAdded);
    int addToRedis(std::string fileMetaData[]);

    int getDirectoryCount();
    std::string getChannelName(int channelID);

    bool fileExists(int channelID, int fileID);

    int getLastChannelID();
    std::string getFileName(int fileID, int channelID);
    int getFileSize(int channelID, int fileID);

    std::string getValue(std::string key);
    void setChannels(std::vector<std::string>& channels);

    void closeConnection();
};