#include "TXMetaDataSender.h"

TXMetaDataSender::TXMetaDataSender(std::string IP, unsigned int port) : TXSender(IP, port),
                                                              directoryReader(FILES_PATH),
                                                              redisHandler(0)
{
    /* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and redisHandler and then the rest of the fields */

    this->metaData = new FileMetaData();
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

    std::string currentPath = "";
    std::string parameters[2];

    Directory* directory = new Directory();

	directory->directoryPath = FILES_PATH;
	directory->dir = opendir(directory->directoryPath.c_str());

	if (directory->dir == NULL)
	{
		throw("Couldn't open directory");
	}

    while ((directory->entry = readdir(directory->dir)) != NULL)
    {
        if (directory->entry->d_type == IS_FILE)
        {
            currentPath = std::string(FILES_PATH) + "/" + directory->entry->d_name;

            this->fileReader.setFile(currentPath.c_str());

            this->metaData->filename = directory->entry->d_name;
            this->metaData->size = this->fileReader.getFileSize();

            this->fileReader.closeFile();

            this->metaData->ID = saveToRedis();

            bufferBuilder();
            sendPacket();
        }
    }

    closedir(directory->dir);
    delete directory;
}

int TXMetaDataSender::saveToRedis()
{
    /* The function will use the RedisHandler object
       to save the file's meta data and will return
       the current file's ID */

    std::string parameters[2];

    parameters[0] = this->metaData->filename;
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
    metaDataBuffer += this->metaData->filename + ",";
    metaDataBuffer += std::to_string(this->metaData->size);

    const char* currentBuffer = metaDataBuffer.c_str();

    std::copy(currentBuffer, currentBuffer + BUFFER_SIZE + 1, this->buffer);
}