#include "RXDataReceiver.h"
#include "RXMetaDataReceiver.h"

#define DATA_PORT 1410
#define META_DATA_PORT 1411

#define PORT_OFFSET 50000
#define PORTS_PER_CHANNEL 10

void preparePorts();

void openMetaDataPorts(std::vector<int> ports, std::vector<std::string> channels);
void openDataPorts(std::vector<int> ports, std::vector<std::string> channels);
