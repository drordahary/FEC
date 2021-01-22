#include "RXDataReceiver.h"
#include "RXMetaDataReceiver.h"
#include "Settings.h"

#define DATA_PORT 1410
#define META_DATA_PORT 1411

#define PORT_OFFSET 50000
#define PORTS_PER_CHANNEL 10

class RX
{
private:
    Settings settings;
    RedisHandler redisHandler;
    std::vector<int> metaDataPorts;
    std::vector<int> dataPorts;
    std::vector<std::string> channels;

public:
    RX();
    ~RX();
    void preparePorts();
    void openMetaDataPorts();
    void openDataPorts();
};
