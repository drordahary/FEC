#pragma once

#include "RXReceiver.h"

class RXDataReceiver : public RXReceiver
{
private:
    FileBuilder fileBuilder;
    Deserializer deserializer;
    

public:
    RXDataReceiver(unsigned int port);
    ~RXDataReceiver();

    void receiveData();
};