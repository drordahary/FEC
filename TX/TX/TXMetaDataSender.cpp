#include "TXMetaDataSender.h"

TXMetaDataSender::TXMetaDataSender(std::string IP, unsigned int port, std::string workingChannel, int channelID, int bufferSize) : TXSender(IP, port, workingChannel, bufferSize),
                                                                                                                                   directoryReader(true),
                                                                                                                                   redisHandler(0)
{
    /* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and redisHandler and then the rest of the fields */

    this->metaData = new FileMetaData();
    this->channelID = channelID;
    TXSender::lastIDUpdated = this->redisHandler.getLastChannelID();
}

TXMetaDataSender::~TXMetaDataSender()
{
    /* The destructor will delete first the metaData and close the socket
       and then will automatically free the allocated memory */

    delete this->metaData;
    close(this->sc);
}

void TXMetaDataSender::sendMetaData()
{
    /* The function will set the meta data it needs
       to send and also will call a function that will
       save the meta data to redis */

    std::string trimmedPath = this->workingChannel.substr(0, this->workingChannel.find_last_of('/'));
    this->directoryReader.loadPath(trimmedPath);
    this->directoryReader.iterateDirectory(this->workingChannel);

    paths = this->directoryReader.getPaths();

    std::string currentPath = "";
    std::string parameters[2];
    int currentID = 0;

    for (auto start = this->paths.begin(); start < this->paths.end(); start++)
    {
        // < Creating the structure in the new folder > //

        createStructure(TOSEND_PATH, *start);

        // < Building the path and set the file to use > //

        currentPath = trimmedPath + "/" + *start;
        this->fileReader.setFile(currentPath.c_str());

        // < Set the meta data and closing the file > //

        this->metaData->filePath = *start;
        this->metaData->size = this->fileReader.getFileSize();
        this->fields.insert({"fileID:" + std::to_string(currentID),
                             *start + ":" + std::to_string(this->metaData->size)});

        this->fileReader.closeFile();
        this->metaData->ID = currentID;

        // < Moving the file, building the buffer and sending the packet > //

        this->directoryReader.moveFile(TOSEND_PATH, *start, trimmedPath);

        bufferBuilder();
        sendPacket();

        currentID++;
    }

    saveToRedis();
    this->redisHandler.closeConnection();
}

void TXMetaDataSender::saveToRedis()
{
    /* The function will use the RedisHandler object
       to save the file's meta data and will return
       the current file's ID */

    std::string key = "channelID:" + std::to_string(this->channelID);
    this->redisHandler.addMetaData(this->fields, key);
    this->fields.clear();
}

void TXMetaDataSender::bufferBuilder()
{
    /* The function will build the buffer based on
       the FileMetaData struct. The function will
       be called only if the struct is full */

    std::string metaDataBuffer = "";

    metaDataBuffer += std::to_string(this->channelID) + ",";
    metaDataBuffer += std::to_string(this->metaData->ID) + ",";
    metaDataBuffer += this->metaData->filePath + ",";
    metaDataBuffer += std::to_string(this->metaData->size);

    const char *currentBuffer = metaDataBuffer.c_str();

    std::copy(currentBuffer, currentBuffer + this->bufferSize + 1, this->buffer);
}