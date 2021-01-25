#pragma once

#include <iostream>
#include <dirent.h>
#include <string.h>
#include <vector>

#define IS_FILE 0x8
#define TOSEND_PATH "./ToSend"
#define MAX_LENGTH 1024

class DirectoryReader
{
private:
    bool isMetaData;
    std::vector<std::string> paths;
    std::string path;

public:
    DirectoryReader(bool isMetaData);
    ~DirectoryReader();

    void clearPaths();

    void iterateDirectory(std::string currentDirectory);
    void setDirectoryPath(std::string newPath);

    void moveFile(std::string newPath, std::string filePath, std::string contatingFolder);

    std::vector<std::string> getPaths();
    void loadPath(std::string path);
};