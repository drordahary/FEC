#include "TXDataSender.h"

TXDataSender::TXDataSender(std::string IP, unsigned int port) : TXSender(IP, port),
																directoryReader(TOSEND_PATH)
{
	/* The constructor will first call the base class constructor
       in order to initialize the socket, then the object
       directoryReader and then the rest of the fields */

	this->serializer = Serializer();
	this->storage = Storage();
}

TXDataSender::~TXDataSender()
{
	/* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

	close(this->sc);
}

void TXDataSender::readFile(int amountToRead, int position)
{
	/* The function will call readFile 
       and the field buffer will be changed */

	this->fileReader.readFile(amountToRead, position, this->buffer);
}

void TXDataSender::sendBurst(std::string *packets)
{
	/* The function will send the
	   storage packets one by one */

	for (int i = 0; i < BURST && !(packets[i].empty()); i++)
	{
		std::copy(packets[i].c_str(), packets[i].c_str() + BUFFER_SIZE + 1, this->buffer);
		sendPacket();
	}
}

void TXDataSender::preparePackets(int filesize, int fileID)
{
	/* This function will calculate how much to read from
	   the file and will prepare the packets to be sent */

	int position = 0;
	int amountToRead = 0;
	int leftToRead = filesize;
	int i = 0;

	while (leftToRead > 0)
	{
		for (i = 0; i < BURST && leftToRead > 0; i++)
		{
			std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

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

			this->storage.addToStorage(this->buffer);

			position += amountToRead;
			leftToRead -= amountToRead;
		}

		sendBurst(this->storage.getStorage());

		if (leftToRead > 0)
		{
			this->storage.clearStorage();
		}
	}

	if (filesize < (BUFFER_SIZE - (HEX_LENGTH * 2)))
	{
		this->storage.clearStorage();
	}
}

void TXDataSender::prepareFiles()
{
	/* This function will iterate over a directory
	   and will prepare each file to be sent */

	std::string currentPath = "";
	int currentFileID = this->lastIDUpdated;

	Directory *directory = new Directory();

	directory->directoryPath = TOSEND_PATH;
	directory->dir = opendir(directory->directoryPath.c_str());

	if (directory->dir == NULL)
	{
		throw("Couldn't open directory");
	}

	while ((directory->entry = readdir(directory->dir)) != NULL)
	{
		if (directory->entry->d_type == IS_FILE)
		{
			currentPath = std::string(TOSEND_PATH) + "/" + directory->entry->d_name;

			this->fileReader.setFile(currentPath.c_str());

			preparePackets(this->fileReader.getFileSize(), currentFileID);
			currentFileID++;

			this->fileReader.closeFile();
		}
	}

	closedir(directory->dir);
	delete directory;
}

void TXDataSender::startSending()
{
	/* The function will be called
       from a main function */

	prepareFiles();
}