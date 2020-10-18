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

#pragma comment(lib,"ws2_32.lib")

#define PORT 1410

#define BUFFER_SIZE 512

void receivePackets(int socket, FileBuilder& fileBuilder, Deserializer deserializer);
