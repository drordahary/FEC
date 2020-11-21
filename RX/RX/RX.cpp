#include "RX.h"

int main()
{
	/* This main function will initialize connections
	   and will try to create the sockets */

	preparePorts();

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

	std::thread metaDataThread(openMetaDataPorts, std::ref(metaDataPorts));
	std::thread daatThread(openDataPorts, std::ref(dataPorts));

	metaDataThread.join();
	daatThread.join();
}

void openMetaDataPorts(std::vector<int> ports)
{
	/* The function will start the meta data listeners
	   on a specific port for each listener */

	std::vector<RXMetaDataReceiver *> receivers;
	std::vector<std::thread> openThreads;

	for (auto port = ports.begin(); port != ports.end(); port++)
	{
		receivers.push_back(new RXMetaDataReceiver(*port));

		std::thread receiverThread(&RXMetaDataReceiver::receiveMetaData, receivers.back());
		openThreads.push_back(std::move(receiverThread));

		std::cout << "Meta Data port number " << *port << " started..." << std::endl;
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
		}

		delete receivers[i];
	}
}

void openDataPorts(std::vector<int> ports)
{
	/* The function will start the data listeners
	   on a specific port for each listener */

	std::vector<RXDataReceiver *> receivers;
	std::vector<std::thread> openThreads;

	for (auto port = ports.begin(); port != ports.end(); port++)
	{
		receivers.push_back(new RXDataReceiver(*port));

		std::thread receiverThread(&RXDataReceiver::receiveData, receivers.back());
		openThreads.push_back(std::move(receiverThread));

		std::cout << "Data port number " << *port << " started..." << std::endl;
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
		}

		delete receivers[i];
	}
}
