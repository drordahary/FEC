#pragma once

#include <vector>
#include <map>

#define _HEX_LENGTH 10

class FileMonitor
{
private:
    int bufferSize;

public:
    FileMonitor(int bufferSize);
    ~FileMonitor();

    int calculateOffset(int fileSize, int packetID, int packetSize);
};