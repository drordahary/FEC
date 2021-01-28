#pragma once

#include "TXDataSender.h"
#include "TXMetaDataSender.h"
#include "Settings.h"

#include <queue>
#include <condition_variable>

#include <iterator>

#define PORTS_PER_CHANNEL 10
#define LOCAL_IP "127.0.0.1"

class ThreadPool
{
private:
    std::string workingChannel;
    std::string destIP;
    int bufferSize;
    int timesToSend;

    std::vector<std::thread> pool;

    std::queue<std::string> availableFiles;
    std::mutex queueMutex;
    std::condition_variable condition;

    Configurations *configs;

    bool poolTerminated;

public:
    ThreadPool(std::string workingChannel, std::vector<int> portRange,
               int channelID, Configurations *configs);
    ~ThreadPool();

    void waitForJob(int port, int channelID);
    void startJob(std::string data, int port, int channelID);
    void addJob(std::string data, int channelID);

    void shutdown();
};