#include "RX.h"

struct sockaddr_in server, cliaddr;
socklen_t slen;
int recv_len;

int main()
{
	/* This main function will initialize connection
	   and will try to create a socket */

	int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{ 
        perror("Socket creation failed");
        exit(EXIT_FAILURE); 
    } 

	memset(&server, 0, sizeof(server));
    memset(&cliaddr, 0, sizeof(cliaddr));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);


	if (bind(sockfd, (const struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Bind failed");
        exit(EXIT_FAILURE);
	}

	std::string filename = "Files/Test.txt";
	FileBuilder fileBuilder(filename.c_str());

	Deserializer deserializer = Deserializer();

	try
	{
		std::cout << "Running..." << std::endl;
		receivePackets(sockfd, fileBuilder, deserializer);
	}

	catch (...)
	{
		std::cout << "Error occured in function receivePackets()" << std::endl;

		fileBuilder.closeFile();
		exit(EXIT_FAILURE);
	}

	fileBuilder.closeFile();
	close(sockfd);
}


void receivePackets(int socket, FileBuilder& file, Deserializer deserializer)
{
	/* This function will listen to incoming
	   UDP packets and will print them */

	char buffer[BUFFER_SIZE + 1];
	slen = sizeof(cliaddr);

	int packetNumber = 0;

	while (true)
	{
		std::fill(buffer, buffer + (BUFFER_SIZE + 1), '\0');

		if (recv_len = recvfrom(socket, buffer, BUFFER_SIZE + 1, 0, (struct sockaddr*) &cliaddr, &slen) < 0)
		{
			std::cout << "Failed to receive" << std::endl;
			exit(EXIT_FAILURE);
		}

		packetNumber = deserializer.deserializePacket(buffer);
		
		file.writeToFile(buffer);
	}
}

