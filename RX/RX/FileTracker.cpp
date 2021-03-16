#include "FileTracker.h"

FileTracker::FileTracker(int bufferSize) : fileMonitor(bufferSize)
{
    /* The constructor will set bufferSize
       and will create the object */

    this->bufferSize = bufferSize;
    this->readFrom = NULL;
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
        if ((std::find(packetsID.begin(), packetsID.end(), packetID)) != packetsID.end())
        {
            return;
        }
    }

    else
    {
        this->untrackedFiles.insert(std::pair<int, std::vector<int>>(fileID, std::vector<int>()));
    }

    this->untrackedFiles.at(fileID).push_back(packetID);
    writeData(channelID, fileID, packetID, buffer);
}

void FileTracker::writeData(int channelID, int fileID, int packetID, const char *buffer)
{
    /* This function will write to the untracked file 
       the meta data of the packet and then the data */

    std::string fileName = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
    slog_trace("untracked file name: %s", fileName.c_str());
    this->fileBuilder.setFile(fileName, 'a');

    std::string metaData = intToHex(packetID) + "," + std::to_string(strnlen(buffer, this->bufferSize)) + "\n";
    this->fileBuilder.appendToFile(metaData.c_str(), metaData.length());

    std::string data = std::string(buffer) + "\n";
    this->fileBuilder.appendToFile(data.c_str(), data.length());

    this->fileBuilder.closeFile();
}

void FileTracker::trackFile(int channelID, int fileID, std::string fileName, int packetSize)
{
    /* This function will only be called if there is an untracked 
       file that received meta data and can be used normally */

    if (this->untrackedFiles.find(fileID) != this->untrackedFiles.end())
    {
        std::string read = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
        slog_trace("file to read from is: %s", read.c_str());
        this->readFrom = fopen(read.c_str(), "rb");

        fseek(this->readFrom, 0, SEEK_END);
        int fileSize = ftell(this->readFrom);
        fseek(this->readFrom, 0, SEEK_SET);
        slog_trace("file size is %d", fileSize);

        std::string newFileName = std::string(FILES_PATH) + "/" + fileName;
        slog_trace("file to write to: %s", newFileName.c_str());
        this->fileBuilder.setFile(newFileName.c_str(), 'w');

        int offset = 0;
        int amountToRead = 0;
        int amountRead = -1;
        int packetID;
        size_t len = 0;
        char *buffer = new char[this->bufferSize + 1]();
        buffer[bufferSize] = '\0';

        while (offset < fileSize)
        {
            /* Reading the packet ID and the size */

            amountRead = -1;
            fseek(this->readFrom, offset, SEEK_SET);

            amountRead = getline(&buffer, &len, this->readFrom);
            if (amountRead == -1)
            {
                slog_error("couldn't read the file: %s", read.c_str());
                return;
            }

            std::string metaData = buffer;
            slog_trace("meta data is: %s", metaData.c_str());
            packetID = std::stoul(metaData.substr(0, metaData.find(',')), nullptr, 16);
            amountToRead = std::stoi(metaData.substr(metaData.find(',') + 1, metaData.length()));

            offset += amountRead;
            std::fill(buffer, buffer + bufferSize, 0);

            /* Reading the actual data and writing it to the file */

            fseek(this->readFrom, offset, SEEK_SET);
            fread(buffer, 1, amountToRead, this->readFrom);
            this->fileBuilder.writeToFile(buffer, amountToRead, fileMonitor.calculateOffset(fileSize, packetID, packetSize));

            this->fileBuilder.flushBuffer();
            offset += amountToRead + 1;
            std::fill(buffer, buffer + bufferSize, 0);
        }

        this->fileBuilder.closeFile();
        delete[] buffer;
    }
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

std::vector<int> FileTracker::getUntrackedPackets(int fileID)
{
    /* The function will return a pair of 
       the file ID and the packets IDs */

    return this->untrackedFiles.at(fileID);
}

std::string intToHex(int packetID)
{
    /* This function will parse the parameter 
	   packetID to a hexadecimal value */

    std::stringstream hexadecimal;

    hexadecimal << "0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << packetID;
    return hexadecimal.str();
}