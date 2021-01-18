#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;

    int currentFileID;

    void handlePacket(int fileID, int channelID);

public:
    RXDataReceiver(unsigned int port, std::string workingChannel);
    ~RXDataReceiver();

    void receiveData();
    void handleData();
};