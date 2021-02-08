#include "FileTracker.h"

FileTracker::FileTracker(int bufferSize) : fileMonitor(bufferSize)
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
        if (std::find(packetsID.begin(), packetsID.end(), packetID) == packetsID.end())
        {
            this->untrackedFiles.at(fileID).push_back(packetID);
        }
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
    this->fileBuilder.setFile(fileName, 'a');

    std::string metaData = intToHex(packetID) + "," + std::to_string(strnlen(buffer, this->bufferSize)) + "\n";
    this->fileBuilder.appendToFile(metaData.c_str(), metaData.length());

    std::string data = std::string(buffer) + "\n";
    this->fileBuilder.appendToFile(data.c_str(), data.length());

    this->fileBuilder.closeFile();
}

void FileTracker::trackFile(int channelID, int fileID, std::string fileName, int packetSize, int fileSize)
{
    /* This function will only be called if there is an untracked 
       file that received meta data and can be used normally */

    std::string read = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
    this->readFrom = fopen(read.c_str(), "rb");

    std::string newFileName = std::string(FILES_PATH) + "/" + fileName;
    this->fileBuilder.setFile(newFileName.c_str(), 'w');

    int offset = 0;
    int amountToRead = 10;
    int amountRead = 0;
    int packetID;
    size_t len = 0;
    char *buffer = new char[this->bufferSize + 1];

    while (offset < fileSize)
    {
        /* Reading the packet ID and the size */

        fseek(this->readFrom, offset, SEEK_SET);
        offset += getline(&buffer, &len, this->readFrom);

        std::string metaData = buffer;
        packetID = std::stoul(metaData.substr(0, metaData.find(',')), nullptr, 16);
        amountToRead = std::stoi(metaData.substr(metaData.find(',') + 1, metaData.length()));

        std::fill(buffer, buffer + strnlen(buffer, bufferSize + 1), '\0');

        /* Reading the actual data and writing it to the file */

        fseek(this->readFrom, offset, SEEK_SET);
        fread(buffer, 1, amountToRead, this->readFrom);
        this->fileBuilder.writeToFile(buffer, amountToRead, fileMonitor.calculateOffset(fileSize, packetID, packetSize));

        this->fileBuilder.flushBuffer();
        offset += amountToRead;
        std::fill(buffer, buffer + strnlen(buffer, bufferSize + 1), '\0');
    }

    this->fileBuilder.closeFile();
    delete buffer;
}

void FileTracker::eraseUntrackedFile(int fileID)
{
    /* This function will erase the file from the 
       untracked files meaning it is now tracked */

    this->untrackedFiles.erase(fileID);
}

bool FileTracker::isFileUntracked(int fileID)
{
    /* The function will return if 
       the file is in the map or not */

    return this->untrackedFiles.find(fileID) != this->untrackedFiles.end();
}

std::string intToHex(int packetID)
{
    /* This function will parse the parameter 
	   packetID to a hexadecimal value */

    std::stringstream hexadecimal;

    hexadecimal << "0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << packetID;
    return hexadecimal.str();
}