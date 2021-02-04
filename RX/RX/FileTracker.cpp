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

void FileTracker::handleUntrackedFile(int channelID, int fileID, int packetID, const char *buffer)
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

    std::string metaData = intToHex(packetID) + "," + std::to_string(strnlen(buffer, this->bufferSize)) + "\n";
    this->fileBuilder.appendToFile(metaData.c_str());

    std::string data = std::string(buffer) + "\n";
    this->fileBuilder.appendToFile(data.c_str());

    this->fileBuilder.closeFile();
}

void FileTracker::trackFile(int channelID, int fileID, int packetID, std::string fileName, int size)
{
    /* This function will only be called if there is an untracked 
       file that received meta data and can be used normally */

    std::string read = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
    this->readFrom = fopen(read.c_str(), "rb");

    std::string newFileName = std::string(FILES_PATH) + "/" + fileName;
    this->fileBuilder.setFile(newFileName.c_str(), "wb");

    int offset = 0;
    int amountToRead = 10;
    int amountRead = 0;
    int packetID;
    size_t len = 0;
    char *buffer = new char[this->bufferSize + 1];

    while (offset < amountRead)
    {
        /* Reading the packet ID and the size */

        fseek(this->readFrom, offset, SEEK_SET);
        getline(&buffer, &len, this->readFrom);

        std::string metaData = buffer;
        packetID = std::stoul(metaData.substr(0, metaData.find(',')), nullptr, 16);
        amountToRead = std::stoi(metaData.substr(metaData.find(','), metaData.length()));

        /* Reading the actual data and writing it to the file */

        offset += len;
        fseek(this->readFrom, offset, SEEK_SET);

        std::fill(buffer, buffer + (bufferSize + 1), '\0');
        fread(&buffer, 1, amountToRead, this->readFrom);
        this->fileBuilder.writeToFile(buffer, amountToRead, /* implement file monitor to calculate offset */);
    }
}

std::string intToHex(int packetID)
{
    /* This function will parse the parameter 
	   packetID to a hexadecimal value */

    std::stringstream hexadecimal;

    hexadecimal << "0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << packetID;
    return hexadecimal.str();
}