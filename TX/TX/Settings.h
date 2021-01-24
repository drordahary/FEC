#pragma once

#include "RedisHandler.h"
#include <string.h>
#include <vector>

#define REDIS_DB 3
#define CONFIGS_COUNT 6

typedef struct Configurations
{
    std::string srcIP;
    std::string dstIP;
    int portOffset;
    int portsPerChannel;
    int timesToSend;
    int bufferSize;
    std::vector<std::string> channels;

} Configurations;

class Settings
{
private:
    RedisHandler redisHandler;
    Configurations *configs;

public:
    Settings();
    ~Settings();
    void fetchResults();
    void organizeResults(std::vector<std::string> results);
    Configurations *getConfigs();
};