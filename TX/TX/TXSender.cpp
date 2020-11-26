#include "TXSender.h"

int TXSender::lastIDUpdated = 0;

TXSender::TXSender(std::string IP, unsigned int port, std::string workingChannel)
{
	/* THe constructor will initialize the socket
       given IP and Port */

	this->workingChannel = workingChannel;

	std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');

	this->slen = sizeof(this->si_other);

	if ((this->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	(this->si_other).sin_family = AF_INET;
	(this->si_other).sin_port = htons(port);
	inet_pton(AF_INET, IP.c_str(), &((this->si_other).sin_addr));
}

TXSender::~TXSender()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void TXSender::sendPacket()
{
	/* This function simply, just receive 
       buffer as a parameter and send it */

	if (sendto(this->sc, this->buffer, BUFFER_SIZE + 1, 0, (struct sockaddr *)&(this->si_other), sizeof(this->si_other)) == -1)
	{
		throw("Failed to send packet");
	}

	std::fill(this->buffer, this->buffer + (BUFFER_SIZE + 1), '\0');
}