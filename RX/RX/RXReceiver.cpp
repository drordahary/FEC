#include "RXReceiver.h"

RXReceiver::RXReceiver(unsigned int port)
{
    /* The constructor will use the the given
       port to initialize the socket */

    if ((this->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ 
        perror("Socket creation failed");
        exit(EXIT_FAILURE); 
    } 

	memset(&(this->server), 0, sizeof((this->server)));
    memset(&(this->cliaddr), 0, sizeof(this->cliaddr));

	(this->server).sin_family = AF_INET;
	(this->server).sin_addr.s_addr = INADDR_ANY;
	(this->server).sin_port = htons(port);


	if (bind(this->sc, (const struct sockaddr *)&(this->server), sizeof(this->server)) < 0)
	{
		perror("Bind failed");
        exit(EXIT_FAILURE);
	}
}

RXReceiver::~RXReceiver()
{
    /* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */
}