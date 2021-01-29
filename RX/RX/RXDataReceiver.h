#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;

    int currentFileID;

    std::map<int, int> receivedFiles;

    std::string handlePacket(int fileID, int channelID);
    int calculateOffset(int fileSize, int packetID, int packetSize);

public:
    RXDataReceiver(unsigned int port, std::string workingChannel, int bufferSize);
    ~RXDataReceiver();

    void receiveData();
    void handleData();
};