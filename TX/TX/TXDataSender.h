#pragma once

#include "TXSender.h"

class TXDataSender : public TXSender
{
private:
    FileReader fileReader;
    Serializer serializer;
    DirectoryReader directoryReader;
    RedisHandler redisHandler;

    void readFile(int amountToRead, int position);

public:
    TXDataSender(std::string IP, unsigned int port, std::string workingChannel, int bufferSize);
    ~TXDataSender();
    void preparePackets(int filesize, int fileID, std::string path, int channelID);
};