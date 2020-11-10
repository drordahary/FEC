#include "RXReceiver.h"

RXReceiver::RXReceiver(unsigned int port)
{
    /* The constructor will use the the given
       port to initialize the socket */

    this->sock = new Socket();
    this->sockfd = this->sock->sc;

    if ((this->sock->sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&(this->sock->server), 0, sizeof((this->sock->server)));
    memset(&(this->sock->cliaddr), 0, sizeof(this->sock->cliaddr));

    (this->sock->server).sin_family = AF_INET;
    (this->sock->server).sin_addr.s_addr = INADDR_ANY;
    (this->sock->server).sin_port = htons(port);

    if (bind(this->sock->sc, (const struct sockaddr *)&(this->sock->server), sizeof(this->sock->server)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

RXReceiver::~RXReceiver()
{
    /* The destructor will close first the socket and the file
       and then will automatically free the allocated memory */

    close(this->sock->sc);
    delete this->sock;
}

void RXReceiver::receivePacket()
{
    /* The function will only receive
       one single packet each time */

    if (this->sock->recv_len = recvfrom(this->sock->sc, this->buffer, sizeof(this->buffer), 0, (struct sockaddr *)&(this->sock->cliaddr), &(this->sock->slen)) < 0)
    {
        std::cout << "Failed to receive" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_lock<std::mutex> lock(this->queueMutex); // Entering critical section

    this->packetsReceived.push(this->buffer);

    lock.unlock();
    this->queueCondition.notify_one(); // Notifying the consumer thread
}