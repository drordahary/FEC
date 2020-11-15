#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;

    int currentFileID;

    void handleFirstPacket(int fileID);
    void handleChangingFile(int fileID);

public:
    RXDataReceiver(unsigned int port);
    ~RXDataReceiver();

    void receiveData();
    void handleData();
};