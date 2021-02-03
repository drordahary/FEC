#include "ThreadPool.h"
#include "TXMetaDataSender.h"
#include "Settings.h"

#define TOSEND_PATH "./ToSend"
#define SLEEP_TIME_MS 500

class TX
{
private:
    Settings settings;
    RedisHandler redisHandler;
    std::vector<int> metaDataPorts;
    std::vector<int> dataPorts;
    std::vector<std::string> channels;
    Configurations *configs;

public:
    TX();
    ~TX();
    void preparePorts();
    void openMetaDataPorts();
    void openDataPorts();
    void workingDataChannel(std::string channel, std::vector<std::string> paths,
                            std::vector<int> ports, int channelID);
};
