#include "TXDataSender.h"

TXDataSender::TXDataSender(std::string IP, unsigned int port,
						   std::string workingChannel, int bufferSize, int timesToSend) : TXSender(IP, port, workingChannel, bufferSize),
																						  directoryReader(false),
																						  redisHandler(0),
																						  serializer(bufferSize)
{
	/* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and then the rest of the fields */

	this->timesToSend = timesToSend;
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

void TXDataSender::preparePackets(int filesize, int fileID, std::string path, int channelID)
{
	/* This function will calculate how much to read from
	   the file and will prepare the packets to be sent */

	for (int i = 0; i < this->timesToSend; i++)
	{
		this->fileReader.setFile(path.c_str());

		int position = 0;
		int amountToRead = 0;
		int leftToRead = filesize;

		while (leftToRead > 0)
		{
			if (leftToRead >= this->bufferSize - (HEX_LENGTH * 3))
			{
				amountToRead = this->bufferSize - (HEX_LENGTH * 3);
			}

			else
			{
				amountToRead = leftToRead;
			}

			readFile(amountToRead, position);
			this->serializer.serializePacket(this->buffer, fileID, channelID);

			sendPacket();

			position += amountToRead;
			leftToRead -= amountToRead;

			std::fill(this->buffer, this->buffer + (this->bufferSize + 1), '\0');
		}

		this->fileReader.closeFile();
		this->serializer.resetPacketCount();
	}

	std::string pathToChannel = path.substr(nthOccurrence(path, "/", 2) + 1, path.find_last_of('/'));
	std::string moveTo = std::string(ARCHIVE_PATH) + "/" + pathToChannel;

	this->directoryReader.moveFile(moveTo, path);
}

int nthOccurrence(const std::string &str, const std::string &find, int nth)
{
	size_t pos = 0;
	int count = 0;

	while (count != nth)
	{
		pos += 1;
		pos = str.find(find, pos);

		if (pos == std::string::npos)
		{
			return -1;
		}

		count++;
	}

	return pos;
}
