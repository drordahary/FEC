#include "FileTracker.h"

FileTracker::FileTracker(int bufferSize) : fileMonitor(bufferSize), readFrom(NULL)
{
    /* The constructor will set bufferSize
       and will create the object */

    this->bufferSize = bufferSize;
}

FileTracker::~FileTracker()
{
    /* The destructor will close the 
       files and deallocate the memory */

    if (this->readFrom.is_open())
    {
        this->readFrom.close();
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

void FileTracker::trackFile(int channelID, int fileID, std::string fileName, int packetSize, int writeToSize)
{
    /* This function will only be called if there is an untracked 
       file that received meta data and can be used normally */

    if (this->untrackedFiles.find(fileID) != this->untrackedFiles.end())
    {
        this->eraseUntrackedFile(fileID);

        std::string read = std::string(UNTRACKED) + "/" + std::to_string(channelID) + std::to_string(fileID);
        slog_trace("file to read from is: %s", read.c_str());

        this->readFrom.open(read, std::ios::binary);
        if (!this->readFrom.is_open())
        {
            return;
        }

        this->readFrom.ignore(std::numeric_limits<std::streamsize>::max());
        int fileSize = this->readFrom.gcount();
        slog_trace("file size is %d", fileSize);
        this->readFrom.clear();
        this->readFrom.seekg(0, std::ios_base::beg);

        std::string newFileName = std::string(FILES_PATH) + "/" + fileName;
        slog_trace("file to write to: %s", newFileName.c_str());
        this->fileBuilder.setFile(newFileName.c_str(), 'w');

        int offset = 0;
        int amountToRead = 0;
        int amountRead = -1;
        int packetID;
        size_t len = 0;
        //char *buffer = new char[this->bufferSize + 1];
        //buffer[bufferSize] = '\0';
        std::vector<char> buffer;
        buffer.resize(bufferSize);
        std::string line;

        while (offset < fileSize)
        {
            /* Reading the packet ID and the size */

            amountRead = 0;
            this->readFrom.seekg(offset);

            std::getline(this->readFrom, line);
            amountRead += line.length() + 1;

            std::string metaData = line;
            slog_trace("meta data is: %s", metaData.c_str());
            packetID = std::stoul(metaData.substr(0, metaData.find(',')), nullptr, 16);
            amountToRead = std::stoi(metaData.substr(metaData.find(',') + 1, metaData.length()));

            offset += amountRead;

            /* Reading the actual data and writing it to the file */

            slog_trace("seeking...");
            this->readFrom.seekg(offset);
            slog_trace("reading...");
            //fread(buffer, 1, amountToRead, this->readFrom);
            this->readFrom.read(&buffer[0], amountToRead);
            slog_trace("data is: %s", buffer.data());
            slog_trace("writing...");
            this->fileBuilder.writeToFile(&buffer[0], amountToRead, fileMonitor.calculateOffset(writeToSize, packetID, packetSize));

            slog_trace("flushing the buffer...");
            this->fileBuilder.flushBuffer();
            offset += amountToRead + 1;
            slog_trace("zeroing the buffer... buffer size is: %d", bufferSize);
            std::fill(buffer.begin(), buffer.end(), '\0');
        }

        slog_info("closing the file");
        this->fileBuilder.closeFile();
        slog_info("finished task");
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