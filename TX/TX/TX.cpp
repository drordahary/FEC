#include "TX.h"

int main()
{
	TX tx = TX();
	tx.preparePorts();

	return 0;
}

TX::TX() : redisHandler(2)
{
	/* This constructor will initialize 
	   Redis handler and the Settings */
}

TX::~TX()
{
	/* This destructor will close 
	   the connection to the database */

	this->redisHandler.closeConnection();
	slog_destroy();
}

void TX::preparePorts()
{
	/* This function will prepare all the ports and
	   the additional information to start sending */

	this->configs = this->settings.getConfigs();

	int dirCount = this->configs->channels.size();
	int multiplier = dirCount / this->configs->portsPerChannel + 1;

	for (int i = 1; i <= dirCount; i++)
	{
		channels.push_back(this->configs->channels[i - 1]);
		metaDataPorts.push_back(this->configs->portOffset + i);

		for (int j = 0; j < this->configs->portsPerChannel; j++)
		{
			dataPorts.push_back(this->configs->portOffset + (multiplier * this->configs->portsPerChannel) + j);
		}

		multiplier++;
	}

	std::thread metaData(&TX::openMetaDataPorts, this);
	metaData.join();
}

void TX::openMetaDataPorts()
{
	/* This function will open all the 
	   meta data ports to start sending */

	std::vector<TXMetaDataSender *> senders;
	std::vector<ThreadPool *> pools;
	std::vector<std::thread> openThreads;

	auto channel = channels.begin();
	int currentChannelID = 0;
	int currentOffset = 0;

	for (auto port = metaDataPorts.begin(); port != metaDataPorts.end(); port++)
	{
		std::vector<int> currentWorkingChannel(dataPorts.begin() + currentOffset,
											   dataPorts.begin() + currentOffset + this->configs->portsPerChannel);

		pools.push_back(new ThreadPool(*channel, currentWorkingChannel, currentChannelID, configs));
		slog_info("thread pool started for port: %d", *port);

		senders.push_back(new TXMetaDataSender(this->configs->dstIP, *port, *channel, currentChannelID, this->configs->bufferSize, pools.back()));

		std::thread senderThread(&TXMetaDataSender::sendMetaData, senders.back());
		openThreads.push_back(std::move(senderThread));
		slog_info("meta data thread port: %d started", *port);

		channel++;
		currentChannelID++;
		currentOffset += this->configs->portsPerChannel;
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
			delete senders[i];
			pools[i]->shutdown();
			delete pools[i];
		}
	}
}
