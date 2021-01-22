#include "ThreadPool.h"
#include "Settings.h"

#define TOSEND_PATH "./ToSend"

#define IP "127.0.0.1"

#define PORT_OFFSET 50000
#define SLEEP_TIME_MS 500

class TX
{
private:
    Settings settings;
    RedisHandler redisHandler;
    std::vector<int> metaDataPorts;
    std::vector<int> dataPorts;
    std::vector<std::string> channels;

public:
    TX();
    ~TX();
    void preparePorts();
    void openMetaDataPorts();
    void openDataPorts();
    void workingDataChannel(std::string channel, std::vector<std::string> paths,
                            std::vector<int> ports, int channelID);
};
