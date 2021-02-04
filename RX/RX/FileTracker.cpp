#include "FileTracker.h"

FileTracker::FileTracker(int bufferSize)
{
    /* The constructor will set bufferSize
       and will create the object */

    this->bufferSize = bufferSize;
}

FileTracker::~FileTracker()
{
    /* The destructor will close the 
       files and deallocate the memory */

    if (this->readFrom != NULL)
    {
        fclose(this->readFrom);
    }

    this->fileBuilder.closeFile();
}

void FileTracker::handleUntrackedFile(int channelID, int fileID, int packetID, const char* buffer)
{
    /* This function will handle files that
       have not any meta data in Redis */

    std::map<int, std::vector<int>>::const_iterator search = untrackedFiles.find(fileID);

    if (search != untrackedFiles.end())
    {
        std::vector<int> packetsID = search->second;
        if (std::find(packetsID.begin(), packetsID.end(), packetID) != packetsID.end())
        {
            return;
        }

        this->untrackedFiles.at(fileID).push_back(packetID);
    }

    else
    {
        this->untrackedFiles.insert(std::pair<int, std::vector<int>>(fileID, std::vector<int>()));
    }

    writeData(channelID, fileID, packetID, buffer);
}

void FileTracker::writeData(int channelID, int fileID, int packetID, const char *buffer)
{
    /* This function will write to the untracked file 
       the meta data of the packet and then the data */

    std::string fileName = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
    this->fileBuilder.setFile(fileName, "a");

    std::string metaData = intToHex(packetID) + std::to_string(strnlen(buffer, this->bufferSize)) + "\n";
    this->fileBuilder.appendToFile(metaData.c_str());

    std::string data = intToHex(packetID) + buffer;
    this->fileBuilder.appendToFile(data.c_str());

    this->fileBuilder.closeFile();
}

void FileTracker::trackFile(int channelID, int fileID, int packetID)
{
    /* This function will only be called if there is an untracked 
       file that received meta data and can be used normally */
    
    std::string fileName = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
    this->readFrom = fopen(fileName.c_str(), "rb");

    int offset = 0;
}

std::string intToHex(int packetID)
{
    /* This function will parse the parameter 
	   packetID to a hexadecimal value */

    std::stringstream hexadecimal;

    hexadecimal << "0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << packetID;
    return hexadecimal.str();
}