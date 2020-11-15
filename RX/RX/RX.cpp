#include "RX.h"

int main()
{
	/* This main function will initialize connection
	   and will try to create a socket */

	RXMetaDataReceiver *metaDataReceiver = new RXMetaDataReceiver(META_DATA_PORT);
	RXDataReceiver *dataReceiver = new RXDataReceiver(DATA_PORT);

	// < Starting the meta data thread > //

	std::thread MDReceiver(&RXMetaDataReceiver::receiveMetaData, metaDataReceiver);

	// < Starting the data thread > //

	std::thread DReceiver(&RXDataReceiver::receiveData, dataReceiver);

	// < Joining the threads > //

	MDReceiver.join();
	DReceiver.join();

	// < Deleting the objects > //

	delete metaDataReceiver;
	delete dataReceiver;

	return 0;
}
