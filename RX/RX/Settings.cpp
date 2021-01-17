#include "Settings.h"

Settings::Settings() : redisHandler(REDIS_DB)
{
    /* The constructor will initialize the configurations struct
       and will call a function that will fetch the results from Redis */

    this->configs = new Configurations();
    fetchResults();
}

Settings::~Settings()
{
    /* The destructor will free the memory of configs
       and all the other memory of the object
       and will also close the connection to Redis */

    delete this->configs;
    this->redisHandler.closeConnection();
}

void Settings::fetchResults()
{
    /* The function will fetch the configurations from Redis
       and will apply those to the struct of configurations */

    std::vector<std::string> results;
    std::string keys[CONFIGS_COUNT] =
        {
            "srcIP",
            "dstIP",
            "portOffset",
            "portsPerChannel",
            "timesToSend",
            "bufferSize"};

    for (int i = 0; i < CONFIGS_COUNT; i++)
    {
        results.push_back(this->redisHandler.getValue(keys[i]));
    }

    organizeResults(results);
}

void Settings::organizeResults(std::vector<std::string>& results)
{
    /* This function will use the data from Redis
       to store it in the configuration struct */

    this->configs->srcIP = results[0];
    this->configs->dstIP = results[1];
    this->configs->portOffset = std::stoi(results[2]);
    this->configs->portsPerChannel = std::stoi(results[3]);
    this->configs->timesToSend = std::stoi(results[4]);
    this->configs->bufferSize = std::stoi(results[5]);
}

Configurations *Settings::getConfigs()
{
    /* This function will return the
       configuration struct */

    return this->configs;
}