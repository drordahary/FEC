#include "TX.h"

int main()
{
	TX tx;
	tx.preparePorts();

	return 0;
}

TX::TX() : redisHandler(2)
{
	/* This constructor will initialize 
	   all the necessary fields */

	this->settings = Settings();
}

TX::~TX()
{
	/* The destructor will deallocate the memory and 
	   will close the connection to Redis */

	this->redisHandler.closeConnection();
}

void TX::preparePorts()
{
	/* This function will prepare all the ports and
	   the additional information to start sending */

	int dirCount = this->redisHandler.getDirectoryCount();

	for (int i = 1; i <= dirCount; i++)
	{
		channels.push_back(this->redisHandler.getChannelName(i));
		this->metaDataPorts.push_back(PORT_OFFSET + i);

		for (int j = 0; j < PORTS_PER_CHANNEL; j++)
		{
			this->dataPorts.push_back(PORT_OFFSET + (i * PORTS_PER_CHANNEL) + j);
		}
	}

	this->redisHandler.closeConnection();

	this->redisHandler = RedisHandler(0);
	int lastUpdatedID = this->redisHandler.getLastFileID() + 1;

	openMetaDataPorts();
	openDataPorts(lastUpdatedID);
}

void TX::openMetaDataPorts()
{
	/* This function will open all the 
	   meta data ports to start sending */

	std::vector<TXMetaDataSender *> senders;
	std::vector<std::thread> openThreads;

	auto channel = this->channels.begin();

	for (auto port = this->metaDataPorts.begin(); port != this->metaDataPorts.end(); port++)
	{
		senders.push_back(new TXMetaDataSender(IP, *port, *channel));

		std::thread senderThread(&TXMetaDataSender::sendMetaData, senders.back());
		openThreads.push_back(std::move(senderThread));

		channel++;
	}

	int size = openThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (openThreads[i].joinable())
		{
			openThreads.at(i).join();
			delete senders[i];
		}
	}
}

void TX::openDataPorts(int lastUpdatedID)
{
	/* This function will open the 
	   data ports to start sending */

	std::vector<std::thread> channelsThreads;
	std::map<std::string, std::vector<std::string>> channelFiles;

	DirectoryReader dirReader(std::string(TOSEND_PATH) + "/" + channels[0], false);
	int currentOffset = 0;

	for (auto channel = this->channels.begin(); channel != this->channels.end(); channel++)
	{
		std::vector<int> currentWorkingChannel(this->dataPorts.begin() + currentOffset, this->dataPorts.begin() + currentOffset + PORTS_PER_CHANNEL);
		currentOffset += PORTS_PER_CHANNEL;

		dirReader.iterateDirectory(std::string(TOSEND_PATH) + "/" + *channel);
		channelFiles.insert({*channel, dirReader.getPaths()});

		std::thread dataSender(&TX::workingDataChannel, this, *channel, channelFiles.at(*channel), currentWorkingChannel, lastUpdatedID);
		channelsThreads.push_back(std::move(dataSender));
	}

	int size = channelsThreads.size();

	for (int i = 0; i < size; i++)
	{
		if (channelsThreads[i].joinable())
		{
			channelsThreads[i].join();
		}
	}
}

void TX::workingDataChannel(std::string channel, std::vector<std::string> paths, std::vector<int> ports, int lastUpdatedID)
{
	/* This function will manage all the ports and 
	   will give each port a specific file to work on */

	this->pool.start(channel, ports);

	for (std::string &path : paths)
	{
		this->pool.addJob(std::string(TOSEND_PATH) + "/" + path + "," + std::to_string(lastUpdatedID));
		lastUpdatedID++;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	this->pool.shutdown();
}