#include "TX.h"

int main()
{
	Settings settings = Settings();
	preparePorts();

	return 0;
}

void preparePorts()
{
	/* This function will prepare all the ports and
	   the additional information to start sending */

	std::vector<int> metaDataPorts;
	std::vector<int> dataPorts;

	std::vector<std::string> channels;
	RedisHandler redisHandler(2);

	int dirCount = redisHandler.getDirectoryCount();

	for (int i = 1; i <= dirCount; i++)
	{
		channels.push_back(redisHandler.getChannelName(i));
		metaDataPorts.push_back(PORT_OFFSET + i);

		for (int j = 0; j < PORTS_PER_CHANNEL; j++)
		{
			dataPorts.push_back(PORT_OFFSET + (i * PORTS_PER_CHANNEL) + j);
		}
	}

	redisHandler.closeConnection();

	redisHandler = RedisHandler(0);
	int lastUpdatedID = redisHandler.getLastFileID() + 1;

	redisHandler.closeConnection();

	openMetaDataPorts(metaDataPorts, channels);
	openDataPorts(dataPorts, channels, lastUpdatedID);
}

void openMetaDataPorts(std::vector<int> metaDataPorts, std::vector<std::string> channels)
{
	/* This function will open all the 
	   meta data ports to start sending */

	std::vector<TXMetaDataSender *> senders;
	std::vector<std::thread> openThreads;

	auto channel = channels.begin();

	for (auto port = metaDataPorts.begin(); port != metaDataPorts.end(); port++)
	{
		senders.push_back(new TXMetaDataSender(IP, *port, *channel));

		std::thread senderThread(&TXMetaDataSender::sendMetaData, senders.back());
		openThreads.push_back(std::move(senderThread));

		channel++;

		std::cout << "Started sending Meta Data on port " << *port << std::endl;
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

void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels, int lastUpdatedID)
{
	/* This function will open the 
	   data ports to start sending */

	std::vector<std::thread> channelsThreads;
	std::map<std::string, std::vector<std::string>> channelFiles;

	DirectoryReader dirReader(std::string(TOSEND_PATH) + "/" + channels[0], false);
	int currentOffset = 0;
	int currentChannelID = 0;

	for (auto channel = channels.begin(); channel != channels.end(); channel++)
	{
		std::vector<int> currentWorkingChannel(dataPorts.begin() + currentOffset, dataPorts.begin() + currentOffset + PORTS_PER_CHANNEL);
		currentOffset += PORTS_PER_CHANNEL;

		dirReader.iterateDirectory(std::string(TOSEND_PATH) + "/" + *channel);
		channelFiles.insert({*channel, dirReader.getPaths()});
		
		std::thread dataSender(workingDataChannel, *channel, channelFiles.at(*channel), currentWorkingChannel, lastUpdatedID, currentChannelID);
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

void workingDataChannel(std::string channel, std::vector<std::string> paths, std::vector<int> ports, int lastUpdatedID, int channelID)
{
	/* This function will manage all the ports and 
	   will give each port a specific file to work on */

	ThreadPool pool(channel, ports, channelID);

	for (std::string &path : paths)
	{
		std::cout << lastUpdatedID << std::endl;
		lock.lock();
		pool.addJob(std::string(TOSEND_PATH) + "/" + path + "," + std::to_string(lastUpdatedID), channelID);
		lastUpdatedID++;
		lock.unlock();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
	pool.shutdown();
}