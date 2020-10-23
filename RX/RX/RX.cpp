#include "RX.h"

int main()
{
	/* This main function will initialize connection
	   and will try to create a socket */

	RXMetaDataReceiver metaDataReceiver(META_DATA_PORT);
	metaDataReceiver.receiveMetaData();

	//RXDataReceiver dataReceiver(DATA_PORT);
	//dataReceiver.receiveData();
}
