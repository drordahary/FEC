#include "TX.h"

int main()
{
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

	openMetaDataPorts(metaDataPorts, channels);
	//openDataPorts(dataPorts, channels);

	redisHandler.closeConnection();
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

		//senders.back()->sendMetaData();

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

			std::cout << "Port " << PORT_OFFSET + i << " closed" << std::endl;
		}
	}
}

void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels)
{
	/* This function will open the 
	   data ports to start sending */
}