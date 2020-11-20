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

void preparePorts()
{
	/* The function will prepare
	   all the ports needed */

	RedisHandler redisHandler = RedisHandler(2);
	int dirCount = redisHandler.getDirectoryCount();

	std::vector<int> metaDataPorts;
	std::vector<int> dataPorts;

	for (int i = 1; i <= dirCount; i++)
	{
		metaDataPorts.push_back(PORT_OFFSET + i);

		for (int j = 0; j < PORTS_PER_CHANNEL; j++)
		{
			dataPorts.push_back(PORT_OFFSET + (i * PORTS_PER_CHANNEL) + j);
		}
	}
}
