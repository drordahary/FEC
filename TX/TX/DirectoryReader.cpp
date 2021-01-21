#include "DirectoryReader.h"

DirectoryReader::DirectoryReader(std::string directoryPath, bool isMetaData)
{
    /* The constructor will assign the directory path
       and will initialize the DIR object 
       by opening the directory */

    this->isMetaData = isMetaData;

    this->directory = new Directory();

    this->directory->directoryPath = directoryPath;
    this->directory->dir = opendir(this->directory->directoryPath.c_str());

    if (this->directory->dir == NULL)
    {
        throw("Couldn't open directory");
    }

    closedir(this->directory->dir);
}

DirectoryReader::~DirectoryReader()
{
    /* The destructor will delete automatically
	   all the allocated memory of the object */

    delete this->directory;
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
                pos = strnlen(FILES_PATH, MAX_LENGTH);
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
    printf("ok1");
    std::string pathToFile = containingFolder + "/" + filePath;
    newPath += "/" + filePath;
    printf("ok2");

    if (rename(pathToFile.c_str(), newPath.c_str()) != 0)
    {
        throw("Couldn't move the file to a new location");
    }
    printf("ok3");
}

void DirectoryReader::setDirectoryPath(std::string newPath)
{
    /* THe function will assign the 
       current path to a new path */

    this->directory->directoryPath = newPath;
}

std::vector<std::string> DirectoryReader::getPaths()
{
    /* The function will return the 
       vector containing the paths */

    return this->paths;
}