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
			dataPorts.push_back(this->configs->portOffset +
								(multiplier * this->configs->portsPerChannel) + j);
		}

		multiplier++;
	}

	redisHandler = RedisHandler(0);
	int lastUpdatedID = redisHandler.getLastChannelID();

	redisHandler.closeConnection();

	openMetaDataPorts();
	openDataPorts();
}

void TX::openMetaDataPorts()
{
	/* This function will open all the 
	   meta data ports to start sending */

	std::vector<TXMetaDataSender *> senders;
	std::vector<std::thread> openThreads;

	auto channel = channels.begin();
	int currentChannelID = 0;

	for (auto port = metaDataPorts.begin(); port != metaDataPorts.end(); port++)
	{
		senders.push_back(new TXMetaDataSender(this->configs->dstIP, *port, *channel, currentChannelID, this->configs->bufferSize));

		std::thread senderThread(&TXMetaDataSender::sendMetaData, senders.back());
		openThreads.push_back(std::move(senderThread));

		channel++;
		currentChannelID++;
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

void TX::openDataPorts()
{
	/* This function will open the 
	   data ports to start sending */

	std::vector<std::thread> channelsThreads;
	std::map<std::string, std::vector<std::string>> channelFiles;

	DirectoryReader dirReader(false);
	int currentOffset = 0;
	int currentChannelID = 0;

	std::string trimmedPath;
	std::string channelPath = std::string(TOSEND_PATH) + "/";

	for (auto channel = channels.begin(); channel != channels.end(); channel++)
	{
		trimmedPath = channel->substr(channel->find_last_of('/') + 1, channel->length());
		std::vector<int> currentWorkingChannel(dataPorts.begin() + currentOffset,
											   dataPorts.begin() + currentOffset + this->configs->portsPerChannel);
		currentOffset += this->configs->portsPerChannel;

		dirReader.iterateDirectory(channelPath + trimmedPath);
		channelFiles.insert({*channel, dirReader.getPaths()});

		std::thread dataSender(&TX::workingDataChannel, this, channelPath + trimmedPath, channelFiles.at(*channel),
							   currentWorkingChannel, currentChannelID);

		channelsThreads.push_back(std::move(dataSender));
		dirReader.clearPaths();
		currentChannelID++;
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

void TX::workingDataChannel(std::string channel, std::vector<std::string> paths,
							std::vector<int> ports, int channelID)
{
	/* This function will manage all the ports and 
	   will give each port a specific file to work on */

	ThreadPool pool(channel, ports, channelID, this->configs);
	int lastUpdatedID = 0;

	for (std::string &path : paths)
	{
		pool.addJob(std::string(TOSEND_PATH) + "/" + path + "," + std::to_string(lastUpdatedID), channelID);
		lastUpdatedID++;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	pool.shutdown();
}