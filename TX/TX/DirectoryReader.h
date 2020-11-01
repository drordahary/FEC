#pragma once

#include <iostream>
#include <dirent.h>
#include <string.h>

#define IS_FILE 0x8

typedef struct Directory
{
    std::string directoryPath;
    struct dirent *entry;
    DIR *dir;

} Directory;

class DirectoryReader
{
private:
    Directory *directory;

    void moveFile(std::string newPath, std::string filename);

public:
    DirectoryReader(std::string directoryPath);
    ~DirectoryReader();

    void iterateDirectory(std::string newPath);
    void setDirectoryPath(std::string newPath);
};