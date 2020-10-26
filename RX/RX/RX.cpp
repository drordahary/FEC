#include "RX.h"

int main()
{
	/* This main function will initialize connection
	   and will try to create a socket */

	RXMetaDataReceiver* metaDataReceiver = new RXMetaDataReceiver(META_DATA_PORT);
	RXDataReceiver* dataReceiver = new RXDataReceiver(DATA_PORT);

	std::thread metaDataThread(&RXMetaDataReceiver::receiveMetaData, metaDataReceiver);
	
	dataReceiver->startReceiving();
	metaDataThread.join();

	delete metaDataReceiver;
	delete dataReceiver;

	return 0;
}
