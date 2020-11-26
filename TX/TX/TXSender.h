#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

#include "FileReader.h"
#include "Serializer.h"
#include "RedisHandler.h"
#include "DirectoryReader.h"
#include "HierarchyBuilder.h"

#define TOSEND_PATH "./ToSend"
#define ARCHIVE_PATH "./Archive"

class TXSender
{
protected:
    int sc;
    struct sockaddr_in si_other;
    int slen;

    char buffer[BUFFER_SIZE + 1];

    std::string workingChannel;

public:
    TXSender(std::string IP, unsigned int port, std::string workingChannel);
    ~TXSender();

    void sendPacket();

    static int lastIDUpdated;
};