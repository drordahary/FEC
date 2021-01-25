#include "DirectoryReader.h"

DirectoryReader::DirectoryReader(bool isMetaData)
{
    /* The constructor will assign the directory path
       and will initialize the DIR object 
       by opening the directory */

    this->isMetaData = isMetaData;
}

DirectoryReader::~DirectoryReader()
{
    /* The destructor will delete automatically
	   all the allocated memory of the object */
}

void DirectoryReader::clearPaths()
{
    /* This function will clear 
       the paths in the vector */

    this->paths.clear();
}

void DirectoryReader::iterateDirectory(std::string currentDirectory)
{
    /* This recursive function will iterate over sub-directories
       as well and will save each path found to a vector */

    DIR *dir = opendir(currentDirectory.c_str());
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (*(entry->d_name) != '.' && entry->d_type != IS_FILE) // Directory found
        {
            std::string path = currentDirectory + "/" + entry->d_name + "/";
            iterateDirectory(path);
        }

        else if (*(entry->d_name) != '.') // File found
        {
            std::string path = (currentDirectory + "/" + entry->d_name);

            int pos;

            if (this->isMetaData)
            {
                pos = strnlen(this->path.c_str(), MAX_LENGTH);
            }

            else
            {
                pos = strnlen(TOSEND_PATH, MAX_LENGTH);
            }

            path = path.substr(pos);

            pos = path.find('/');

            if (pos == -1)
            {
                this->paths.push_back(entry->d_name);
            }

            else
            {
                path = path.substr(pos + 1);
                this->paths.push_back(path);
            }
        }
    }

    closedir(dir);
}

void DirectoryReader::moveFile(std::string newPath, std::string filePath, std::string containingFolder)
{
    /* The function will move the files from the 
       current opened directory to the new path */

    std::string pathToFile = containingFolder + "/" + filePath;
    newPath += "/" + filePath;

    if (rename(pathToFile.c_str(), newPath.c_str()) != 0)
    {
        std::cout << "Couldn't move the file to a new location" << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::vector<std::string> DirectoryReader::getPaths()
{
    /* The function will return the 
       vector containing the paths */

    return this->paths;
}

void DirectoryReader::loadPath(std::string path)
{
    /* This function will load a path to use as 
       an offset to files on the channel */
    
    this->path = path;
}