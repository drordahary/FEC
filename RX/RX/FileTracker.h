#include <stdio.h>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include "FileMonitor.h"
#include "FileBuilder.h"

#define UNTRACKED "./Untracked"

class FileTracker
{
private:
    FileBuilder fileBuilder;
    FILE *readFrom;
    std::map<int, std::vector<int>> untrackedFiles;
    int bufferSize;

public:
    FileMonitor fileMonitor;
    FileTracker(int bufferSize);
    ~FileTracker();

    void handleUntrackedFile(int channelID, int fileID, int packetID, const char *buffer);
    void trackFile(int channelID, int fileID, std::string fileName, int packetSize);

    void writeData(int channelID, int fileID, int packetID, const char *buffer);
    void eraseUntrackedFile(int fileID);
    bool isFileUntracked(int fileID);
    std::vector<int> getUntrackedPackets(int fileID);
};

std::string intToHex(int packetID);