#include "RX.h"

int main()
{
	/* This main function will initialize connections
	   and will try to create the sockets */

	RX rx = RX();
	rx.preparePorts();

	return 0;
}

RX::RX() : redisHandler(2)
{
	/* This constructor will initialize
	   Redis handler and Settings */
}

RX::~RX()
{
	/* The destructor will close the 
	   connection to the database */

	this->redisHandler.closeConnection();
}

void RX::preparePorts()
{
	/* The function will prepare
	   all the ports needed */

	this->configs = settings.getConfigs();
	int dirCount = this->configs->channels.size();
	int multiplier = dirCount / this->configs->portsPerChannel + 1;

	for (int i = 1; i <= dirCount; i++)
	{
		channels.push_back(this->configs->channels[i - 1]);
		metaDataPorts.push_back(this->configs->portOffset + i);

		for (int j = 0; j < this->configs->portsPerChannel; j++)
		{
			dataPorts.push_back(this->configs->portOffset +
								(multiplier * this->configs->portsPerChannel) + j);
		}

		multiplier++;
	}

	std::thread metaDataThread(&RX::openMetaDataPorts, this);
	std::thread dataThread(&RX::openDataPorts, this);

	metaDataThread.join();
	dataThread.join();
}

void RX::openMetaDataPorts()
{
	/* The function will start the meta data listeners
	   on a specific port for each listener */

	std::vector<RXMetaDataReceiver *> receivers;
	std::vector<std::thread> openThreads;

	auto channel = channels.begin();

	for (auto port = metaDataPorts.begin(); port != metaDataPorts.end(); port++)
	{
		receivers.push_back(new RXMetaDataReceiver(*port, *channel, this->configs->bufferSize));

		std::thread receiverThread(&RXMetaDataReceiver::receiveMetaData, receivers.back());
		openThreads.push_back(std::move(receiverThread));

		channel++;

		std::cout << "Meta Data port number " << *port << " started..." << std::endl;
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
			delete receivers[i];
		}
	}
}

void RX::openDataPorts()
{
	/* The function will start the data listeners
	   on a specific port for each listener */

	std::vector<RXDataReceiver *> receivers;
	std::vector<std::thread> openThreads;

	auto channel = channels.begin();
	auto port = dataPorts.begin();

	for (auto channel = channels.begin(); channel != channels.end(); channel++)
	{
		for (int i = 0; i < this->configs->portsPerChannel; i++)
		{
			receivers.push_back(new RXDataReceiver(*port, *channel, this->configs->bufferSize));

			std::thread receiverThread(&RXDataReceiver::receiveData, receivers.back());
			openThreads.push_back(std::move(receiverThread));

			std::cout << "Data port number " << *port << " started..." << std::endl;
			port++;
		}
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
			delete receivers[i];
		}
	}
}
