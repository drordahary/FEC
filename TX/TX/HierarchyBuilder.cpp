#include "HierarchyBuilder.h"

void createStructure(std::string newPath, std::string path)
{
    /* This function will create the folders structure
       (the hierarchy) so it matches the same
       structure as the original folder */

    int pos;
    pos = path.find('/');

    if (pos != -1)
    {
        createFolder(path, newPath + "/");
    }
}

void createFolder(std::string left, std::string currentPath)
{
    /* This recursive function will create only 
       the folder with a specific structure */

    if (left[0] == '/')
    {
        left.erase(0, 1);
    }

    int pos = left.find('/');

    if (pos == -1)
    {
        return;
    }

    currentPath = currentPath + left.substr(0, pos) + "/";
    mkdir(currentPath.c_str(), 0777);

    createFolder(left.substr(pos + 1), currentPath);
}