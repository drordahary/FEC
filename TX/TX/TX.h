#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include <hiredis/hiredis.h>

#include "FileReader.h"
#include "Serializer.h"
#include "Storage.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 44664
#define IP "127.0.0.1"

void preparePackets(int sc, FileReader& reader, Serializer serializer, Storage& storage, int filesize);
void sendBurst(int sc, Storage storage);
void readFile(FileReader& reader, char buffer[], int amountToRead, int position);