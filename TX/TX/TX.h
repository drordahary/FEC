#include "ThreadPool.h"
#include "Settings.h"

#define TOSEND_PATH "./ToSend"

#define IP "127.0.0.1"

#define PORT_OFFSET 50000
#define SLEEP_TIME_MS 500 


std::mutex lock;

void preparePorts();

void openMetaDataPorts(std::vector<int> metaDataPorts, std::vector<std::string> channels);
void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels, int lastUpdatedID);

void workingDataChannel(std::string channel, std::vector<std::string> paths, std::vector<int> ports, int lastUpdatedID);