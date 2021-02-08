#include "TXSender.h"

int TXSender::lastIDUpdated = 0;

TXSender::TXSender(std::string ip, unsigned int port, std::string workingChannel, int bufferSize)
{
	/* THe constructor will initialize the socket
       given IP and Port */

	this->workingChannel = workingChannel;
	this->bufferSize = bufferSize;

	this->buffer = new char[bufferSize + 1];

	std::fill(this->buffer, this->buffer + (bufferSize + 1), '\0');

	this->slen = sizeof(this->si_other);

	if ((this->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	(this->si_other).sin_family = AF_INET;
	(this->si_other).sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &((this->si_other).sin_addr));
}

TXSender::~TXSender()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */

	delete this->buffer;
}

void TXSender::sendPacket()
{
	/* This function simply, just receive 
       buffer as a parameter and send it */

	if (sendto(this->sc, this->buffer, this->bufferSize + 1, 0, (struct sockaddr *)&(this->si_other), sizeof(this->si_other)) == -1)
	{
		slog_fatal("failed to send packet");
	}

	std::fill(this->buffer, this->buffer + (this->bufferSize + 1), '\0');
}

void TXSender::sendPacket(int packetID)
{
	/* This function simply, just receive 
       buffer as a parameter and send it */

	if (sendto(this->sc, this->buffer, this->bufferSize + 1, 0, (struct sockaddr *)&(this->si_other), sizeof(this->si_other)) == -1)
	{
		slog_fatal("failed to send packet");
	}

	slog_info("packet ID: %d sent", packetID);
	std::fill(this->buffer, this->buffer + (this->bufferSize + 1), '\0');
}