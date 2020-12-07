#include "TXDataSender.h"

TXDataSender::TXDataSender(std::string IP, unsigned int port, std::string workingChannel) : TXSender(IP, port, workingChannel),
																							directoryReader(TOSEND_PATH, false),
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

void TXDataSender::preparePackets(int filesize, int fileID, std::string path)
{
	/* This function will calculate how much to read from
	   the file and will prepare the packets to be sent */

	this->fileReader.setFile(path.c_str());

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

	this->fileReader.closeFile();
}
