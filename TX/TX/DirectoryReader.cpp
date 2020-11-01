#include "DirectoryReader.h"

DirectoryReader::DirectoryReader(std::string directoryPath)
{
    /* The constructor will assign the directory path
       and will initialize the DIR object 
       by opening the directory */

    this->directory = new Directory();

    this->directory->directoryPath = directoryPath;
    this->directory->dir = opendir(this->directory->directoryPath.c_str());

    if (this->directory->dir == NULL)
    {
        throw("Couldn't open directory");
    }
}

DirectoryReader::~DirectoryReader()
{
    /* The destructor will delete automatically
	   all the allocated memory of the object */

    delete this->directory;
}

void DirectoryReader::iterateDirectory(std::string newPath)
{
    /* The function will iterate a directory 
       and will use the function moveFiles */

    while ((this->directory->entry = readdir(this->directory->dir)) != NULL)
    {
        if (this->directory->entry->d_type == IS_FILE)
        {
            moveFile(newPath, this->directory->entry->d_name);
        }
    }

    closedir(this->directory->dir);
}

void DirectoryReader::moveFile(std::string newPath, std::string filename)
{
    /* The function will move the files from the 
       current opened directory to the new path */

    std::string pathToFile = this->directory->directoryPath + "/" + filename;
    newPath += "/" + filename;

    if (rename(pathToFile.c_str(), newPath.c_str()) != 0)
    {
        throw("Couldn't move the file to a new location");
    }
}

void DirectoryReader::setDirectoryPath(std::string newPath)
{
    /* THe function will assign the 
       current path to a new path */

    this->directory->directoryPath = newPath;
}