#include "TX.h"

struct sockaddr_in si_other;
int slen = sizeof(si_other);

int main()
{
	/* This main function will initialize the connection
	   and all the necessary objects
	   and will try to create a socket */

	DirectoryReader directoryReader = DirectoryReader("/home/magshimim/Documents/Files");
	directoryReader.iterateDirectory("./ToSend");

	RedisHandler handler = RedisHandler(0);

	int sockfd;

	slen = sizeof(si_other);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
        exit(EXIT_FAILURE);
	}

	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &(si_other.sin_addr));

	std::string filepath = "/home/magshimim/Files/Read.txt";
	FileReader reader(filepath.c_str());

	Serializer serializer = Serializer();
	Storage storage = Storage();

	try
	{
		preparePackets(sockfd, reader, serializer, storage, reader.getFileSize());
	}

	catch (std::string error)
	{
		std::cout << error << std::endl;
	}

	reader.closeFile();
	close(sockfd);
}

void sendBurst(int sc, std::string* packets)
{
	/* The function will send the
	   storage packets one by one */

	int i = 0;

	for (i = 0; i < BURST && !(packets[i].empty()); i++)
	{
		if (sendto(sc, packets[i].c_str(), BUFFER_SIZE + 1, 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
		{
			throw ("Failed to send packet");
		}
	}
}

void preparePackets(int sc, FileReader& reader, Serializer serializer, Storage& storage, int filesize)
{
	/* This function will calculate how much to read from
	   the file and will prepare the packets to be sent */

	char buffer[BUFFER_SIZE + 1];

	int position = 0;
	int amountToRead = 0;
	int leftToRead = filesize;
	int i = 0;

	while (leftToRead > 0)
	{
		for (i = 0; i < BURST && leftToRead > 0; i++)
		{
			memset(buffer, '\0', BUFFER_SIZE + 1);

			if (leftToRead >= BUFFER_SIZE - HEX_LENGTH)
			{
				amountToRead = BUFFER_SIZE - HEX_LENGTH;
			}

			else
			{
				amountToRead = leftToRead;
			}

			readFile(reader, buffer, amountToRead, position);
			serializer.serializePacket(buffer);

			storage.addToStorage(buffer);

			position += amountToRead;
			leftToRead -= amountToRead;
		}

		i = 0;

		sendBurst(sc, storage.getStorage());

		if (leftToRead > 0)
		{
			storage.clearStorage();
		}
	}
}

void readFile(FileReader& reader, char buffer[], int amountToRead, int position)
{
	reader.readFile(amountToRead, position, buffer);
}
