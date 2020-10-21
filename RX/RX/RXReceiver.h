#pragma once

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>

#include "FileBuilder.h"
#include "Deserializer.h"

#define FILES_PATH "./Files/Test.txt"

class RXReceiver
{
protected:
    int sc;
    struct sockaddr_in server, cliaddr;
    socklen_t slen;
    int recv_len;

public:
    RXReceiver(unsigned int port);
    ~RXReceiver();
};