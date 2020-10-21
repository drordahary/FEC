#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    char buffer[BUFFER_SIZE + 1];

public:
    RXDataReceiver(unsigned int port);
    ~RXDataReceiver();

    void receivePackets();
};