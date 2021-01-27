#include "RXDataReceiver.h"
#include "RXMetaDataReceiver.h"
#include "Settings.h"

class RX
{
private:
    Settings settings;
    RedisHandler redisHandler;
    std::vector<int> metaDataPorts;
    std::vector<int> dataPorts;
    std::vector<std::string> channels;
    Configurations *configs;

public:
    RX();
    ~RX();
    void preparePorts();
    void openMetaDataPorts();
    void openDataPorts();
};
