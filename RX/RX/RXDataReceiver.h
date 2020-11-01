#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    RedisHandler redisHandler;

    int currentFileID;

public:
    RXDataReceiver(unsigned int port);
    ~RXDataReceiver();

    void receiveData();

    void startReceiving();
};