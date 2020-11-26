#include "TXDataSender.h"
#include "TXMetaDataSender.h"

#define IP "127.0.0.1"

#define PORT_OFFSET 50000
#define PORTS_PER_CHANNEL 10

std::mutex lock;

void preparePorts();

void openMetaDataPorts(std::vector<int> metaDataPorts, std::vector<std::string> channels);
void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels);