#include "TXMetaDataSender.h"

TXMetaDataSender::TXMetaDataSender(std::string IP, unsigned int port) : TXSender(IP, port),
                                                                        directoryReader(FILES_PATH),
                                                                        redisHandler(0)
{
    /* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and redisHandler and then the rest of the fields */

    this->metaData = new FileMetaData();
    TXSender::lastIDUpdated = this->redisHandler.getLastFileID() + 1;
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

    this->directoryReader.iterateDirectory(FILES_PATH);
    paths = this->directoryReader.getPaths();

    std::string currentPath = "";
    std::string parameters[2];

    for (auto start = this->paths.begin(); start < this->paths.end(); start++)
    {
        // < Creating the structure in the new folder > //

        createStructure(TOSEND_PATH, *start);

        // < Building the path and set the file to use > //

        currentPath = std::string(FILES_PATH) + "/" + *start;
        this->fileReader.setFile(currentPath.c_str());

        // < Set the meta data and closing the file > //

        this->metaData->filePath = *start;
        this->metaData->size = this->fileReader.getFileSize();

        this->fileReader.closeFile();
        this->metaData->ID = saveToRedis();

        // < Moving the file, building the buffer and sending the packet > //

        this->directoryReader.moveFile(TOSEND_PATH, *start);

        bufferBuilder();
        sendPacket();
    }

    this->redisHandler.closeConnection();
}

void TXMetaDataSender::createStructure(std::string newPath, std::string path)
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

int TXMetaDataSender::saveToRedis()
{
    /* The function will use the RedisHandler object
       to save the file's meta data and will return
       the current file's ID */

    std::string parameters[2];

    parameters[0] = this->metaData->filePath;
    parameters[1] = std::to_string(this->metaData->size);

    int ID = this->redisHandler.addToRedis(parameters);

    return ID;
}

void TXMetaDataSender::bufferBuilder()
{
    /* The function will build the buffer based on
       the FileMetaData struct. The function will
       be called only if the struct is full */

    std::string metaDataBuffer = "";

    metaDataBuffer += std::to_string(this->metaData->ID) + ",";
    metaDataBuffer += this->metaData->filePath + ",";
    metaDataBuffer += std::to_string(this->metaData->size);

    const char *currentBuffer = metaDataBuffer.c_str();

    std::copy(currentBuffer, currentBuffer + BUFFER_SIZE + 1, this->buffer);
}