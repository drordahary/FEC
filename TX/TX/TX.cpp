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
	openDataPorts(dataPorts, channels);

	redisHandler.closeConnection();
}

void openMetaDataPorts(std::vector<int> metaDataPorts, std::vector<std::string> channels)
{
	/* This function will open all the 
	   meta data ports to start sending */

	std::vector<TXMetaDataSender *> senders;
	auto channel = channels.begin();

	for (auto port = metaDataPorts.begin(); port != metaDataPorts.end(); port++)
	{
		senders.push_back(new TXMetaDataSender(IP, *port, *channel));
		std::thread(&TXMetaDataSender::sendMetaData, senders.back()).detach();

		channel++;

		std::cout << "Started sending meta data on port " << *port << std::endl;
	}

	int size = senders.size();

	for (int i = 0; i < size; i++)
	{
		delete senders[i];
	}
}

void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels)
{
	/* This function will open the 
	   data ports to start sending */
}