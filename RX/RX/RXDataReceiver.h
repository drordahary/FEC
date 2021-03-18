#pragma once

#include <algorithm>
#include "RXReceiver.h"
#include "FileMonitor.h"
#include "FileTracker.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;
    FileTracker fileTracker;
    int lastUpdatedPacketID;

    std::mutex mLock;

    std::map<int, std::vector<int>> receivedFiles;
    std::map<int, bool> handlingThreads;

    void assumeCase();

public:
    RXDataReceiver(unsigned int port, std::string workingChannel, int bufferSize);
    ~RXDataReceiver();

    void receiveData();
    void handleData(int channelID, int fileID, int packetID, int packetSize);
    void checkUntrackedFile(int channelID, int fileID, int packetID, int packetSize);
};