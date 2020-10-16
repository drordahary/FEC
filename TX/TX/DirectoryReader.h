#pragma once

#include <iostream>
#include <dirent.h>
#include <string.h>

class DirectoryReader
{
private:
    std::string directoryPath;
    struct dirent* entry;
    DIR* directory;

    const unsigned char isFile = 0x8;

    void moveFile(std::string newPath, std::string filename);

public:
    DirectoryReader(std::string directoryPath);
    ~DirectoryReader();

    void iterateDirectory(std::string newPath);
    void setDirectoryPath(std::string newPath);
};