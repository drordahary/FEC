#pragma once

#include <iostream>
#include <dirent.h>
#include <string.h>
#include <vector>

#define IS_FILE 0x8
#define TOSEND_PATH "./ToSend"
#define FILES_PATH "/home/magshimim/Documents/Files"
#define MAX_LENGTH 1024

typedef struct Directory
{
    std::string directoryPath;
    struct dirent *entry;
    DIR *dir;

} Directory;

class DirectoryReader
{
private:
    bool isMetaData;
    std::string directoryPath;
    Directory *directory;
    std::vector<std::string> paths;
    std::string path;

public:
    DirectoryReader(std::string directoryPath, bool isMetaData);
    ~DirectoryReader();

    void clearPaths();

    void iterateDirectory(std::string currentDirectory);
    void setDirectoryPath(std::string newPath);

    void moveFile(std::string newPath, std::string filePath, std::string contatingFolder);

    std::vector<std::string> getPaths();
    void loadPath(std::string path);
};