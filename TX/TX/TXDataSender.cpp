#include "TXDataSender.h"

TXDataSender::TXDataSender(std::string IP, unsigned int port) : TXSender(IP, port),
																directoryReader(TOSEND_PATH),
																redisHandler(0)
{
	/* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and then the rest of the fields */

	this->serializer = Serializer();
}

TXDataSender::~TXDataSender()
{
	/* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

	close(this->sc);
	this->redisHandler.closeConnection();
}

void TXDataSender::readFile(int amountToRead, int position)
{
	/* The function will call readFile 
       and the field buffer will be changed */

	this->fileReader.readFile(amountToRead, position, this->buffer);
}

void TXDataSender::preparePackets(int filesize, int fileID)
{
	/* This function will calculate how much to read from
	   the file and will prepare the packets to be sent */

	int position = 0;
	int amountToRead = 0;
	int leftToRead = filesize;

	while (leftToRead > 0)
	{
		if (leftToRead >= BUFFER_SIZE - (HEX_LENGTH * 2))
		{
			amountToRead = BUFFER_SIZE - (HEX_LENGTH * 2);
		}

		else
		{
			amountToRead = leftToRead;
		}

		readFile(amountToRead, position);
		this->serializer.serializePacket(this->buffer, fileID);

		sendPacket();

		position += amountToRead;
		leftToRead -= amountToRead;

		std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');
	}
}

void TXDataSender::prepareFiles()
{
	/* This function will iterate over redis saved IDs and will
	   use these IDs to get the file's path and send the data */

	std::string currentPath = "";
	std::string fileName = "";

	int currentFileID = this->lastIDUpdated;
	int lastID = this->redisHandler.getLastFileID();

	while (currentFileID <= lastID)
	{
		// < Building the file path and set the file to be used > //

		fileName = this->redisHandler.getFileName(currentFileID);

		currentPath = std::string(TOSEND_PATH) + "/" + fileName;
		this->fileReader.setFile(currentPath.c_str());

		// < Calling the function that handles sending the file's data > //

		for (int i = 0; i < SEND_TIMES; i++)
		{
			preparePackets(this->fileReader.getFileSize(), currentFileID);
		}

		this->fileReader.closeFile();

		// < Creating the structure (hierarchy) and moving the file > //

		createStructure(ARCHIVE_PATH, fileName);
		this->directoryReader.moveFile(ARCHIVE_PATH, fileName, TOSEND_PATH);

		// < Increamenting the ID to move to the next file > //

		currentFileID++;
	}
}

void TXDataSender::startSending()
{
	/* The function will be called
       from a main function */

	prepareFiles();
}