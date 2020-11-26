#include "TXDataSender.h"
#include "TXMetaDataSender.h"

#define IP "127.0.0.1"

#define DATA_PORT 1410
#define META_DATA_PORT 1411

#define PORT_OFFSET 50000
#define PORTS_PER_CHANNEL 10

void preparePorts();

void openMetaDataPorts(std::vector<int> metaDataPorts, std::vector<std::string> channels);
void openDataPorts(std::vector<int> dataPorts, std::vector<std::string> channels);