#pragma once

#include "TXSender.h"

#define SEND_TIMES 2

class TXDataSender : public TXSender
{
private:
    FileReader fileReader;
    Serializer serializer;
    DirectoryReader directoryReader;
    RedisHandler redisHandler;

    void readFile(int amountToRead, int position);

    void preparePackets(int filesize, int fileID);
    void prepareFiles();

public:
    TXDataSender(std::string IP, unsigned int port);
    ~TXDataSender();

    void startSending();
};