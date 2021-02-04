#include <stdio.h>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
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
    FileTracker(int bufferSize);
    ~FileTracker();

    void handleUntrackedFile(int channelID, int fileID, int packetID, const char *buffer);
    void trackFile(int channelID, int fileID, int packetID);

    void writeData(int channelID, int fileID, int packetID, const char *buffer);
};

std::string intToHex(int packetID);