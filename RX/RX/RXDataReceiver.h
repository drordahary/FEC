#pragma once

#include <algorithm>
#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;

    int currentFileID;

    std::map<int, std::vector<int>> receivedFiles;
    std::map<int, bool> fullyReceivedFiles;

    std::string handlePacket(int fileID, int channelID);
    int calculateOffset(int fileSize, int packetID, int packetSize);
    void checkMaxSize(int fileID, int fileSize);

public:
    RXDataReceiver(unsigned int port, std::string workingChannel, int bufferSize);
    ~RXDataReceiver();

    void receiveData();
    void handleData();
};