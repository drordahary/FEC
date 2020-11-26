#pragma once

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <thread>
#include <vector>
#include <sys/select.h>

#include "FileBuilder.h"
#include "Deserializer.h"
#include "RedisHandler.h"
#include "HierarchyBuilder.h"

#define FILES_PATH "./Files"

typedef struct Socket
{
    int sc;
    struct sockaddr_in server, cliaddr;
    socklen_t slen;
    int recv_len;

} Socket;

class RXReceiver
{
protected:
    Socket* sock;
    int sockfd;
    char buffer[BUFFER_SIZE + 1];

    std::string workingChannel;

    void receivePacket();

public:
    RXReceiver(unsigned int port, std::string workingChannel);
    ~RXReceiver();
};