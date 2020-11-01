#pragma once

#include "TXSender.h"

class TXDataSender : public TXSender
{
private:
    FileReader fileReader;
    Serializer serializer;
    Storage storage;
    DirectoryReader directoryReader;

    void readFile(int amountToRead, int position);

    void preparePackets(int filesize, int fileID);
    void sendBurst(std::string* packets);

    void prepareFiles();

public:
    TXDataSender(std::string IP, unsigned int port);
    ~TXDataSender();
    
    void startSending();
};