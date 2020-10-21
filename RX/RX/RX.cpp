#include "RX.h"

int main()
{
	/* This main function will initialize connection
	   and will try to create a socket */

	RXDataReceiver dataReceiver(DATA_PORT);
	dataReceiver.receivePackets();
}
