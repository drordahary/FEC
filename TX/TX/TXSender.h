#pragma once

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include "FileReader.h"
#include "Serializer.h"
#include "Storage.h"
#include "RedisHandler.h"
#include "DirectoryReader.h"

#define FILES_PATH "/home/magshimim/Documents/Files"
#define TOSEND_PATH "./ToSend"
#define ARCHIVE_PATH "./Archive"

class TXSender
{
protected:
    int sc;
    struct sockaddr_in si_other;
    int slen;

    char buffer[BUFFER_SIZE + 1];
    

public:
    TXSender(std::string IP, unsigned int port);
    ~TXSender();

    void sendPacket();

    static int lastIDUpdated;
};