#include "RedisHandler.h"

std::mutex RedisHandler::mLock;

RedisHandler::RedisHandler(int databaseID)
{
    /* The constructor will initialize the database
       ID field and will connect to redis */

    this->databaseID = databaseID;
    connectToRedis();
}

RedisHandler::~RedisHandler()
{
    /* THe destructor will free the context object manually
       and all the other allocated memory automatically */

    redisFree(this->context);
}

void RedisHandler::connectToRedis()
{
    /* The function will connect to redis and 
       select a specific database to use */

    this->context = redisConnect("127.0.0.1", 6379);

    if (!context)
    {
        std::cout << "Couldn't connect to redis (Check if the server is up)" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string command = "select " + std::to_string(this->databaseID);

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution(command);

    freeReplyObject(this->reply);
    slog_info("initialized connection to redis");
}

bool RedisHandler::fileExists(int channelID, int fileID)
{
    /* The function will check if the 
       file ID is present in the database */

    std::string command = "hmget channelID:" + std::to_string(channelID) + " fileID:" + std::to_string(fileID);
    this->reply = (redisReply *)redisCommand(this->context, command.c_str());

    slog_trace("reply returned with %s", reply->element[0]->str);
    bool exists = reply->element[0]->str != NULL;
    freeReplyObject(this->reply);
    return exists;
}

void RedisHandler::addMetaData(std::map<std::string, std::string> &fields, std::string &key, bool channelIDAdded)
{
    /* This function will update Redis with a set of files metadata
       and will use the command HMSET for that operation */

    slog_trace("key is %s", key.c_str());
    std::vector<const char *> argv;
    std::vector<size_t> arglen;

    char command[] = "HMSET";
    argv.push_back(command);
    arglen.push_back(sizeof(command) - 1);

    argv.push_back(key.c_str());
    arglen.push_back(key.size());

    std::map<std::string, std::string>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i)
    {
        argv.push_back(i->first.c_str());
        arglen.push_back(i->first.size());

        argv.push_back(i->second.c_str());
        arglen.push_back(i->second.size());
    }

    void *r = redisCommandArgv(this->context, argv.size(), &(argv[0]), &(arglen[0]));
    if (!r)
    {
        std::cout << "Couldn't perform operation" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeReplyObject(r);

    slog_trace("channelIDAdded is: %d", channelIDAdded);
    if (!channelIDAdded)
    {
        this->reply = (redisReply *)redisCommand(this->context, "incr currentAmount");
        checkExecution(command);
        freeReplyObject(this->reply);
    }
}

int RedisHandler::getLastChannelID()
{
    /* The function will return the last
       (largest) file ID number in redis */

    this->reply = (redisReply *)redisCommand(this->context, "get currentAmount");

    if (!this->reply || this->context->err || this->reply->type != REDIS_REPLY_STRING)
    {
        slog_fatal("could not read from redis");
        exit(EXIT_FAILURE);
    }

    slog_trace("reply returned with: %s", this->reply->str);
    int lastChannelID = atoi(this->reply->str);

    freeReplyObject(this->reply);
    return lastChannelID;
}

int RedisHandler::getDirectoryCount()
{
    /* The function will return 
       the amount of channels */

    std::string command = "get dirCount";

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());

    if (!this->reply || this->context->err || this->reply->type != REDIS_REPLY_STRING)
    {
        slog_fatal("could not read from redis");
        exit(EXIT_FAILURE);
    }

    slog_trace("reply returned with: %s", this->reply->str);
    int dirCount = atoi(this->reply->str);

    freeReplyObject(this->reply);
    return dirCount;
}

std::string RedisHandler::getChannelName(int channelID)
{
    /* The function will return the channel
       name based on the given ID */

    std::string command = "get dirID:" + std::to_string(channelID);

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution(command);

    std::string channelName = this->reply->str;
    freeReplyObject(this->reply);

    return channelName;
}

std::string RedisHandler::getFileName(int fileID, int channelID)
{
    /* The function will receive a file ID to
       search for and will return the file name */

    std::string fileName;

    std::string command = "hmget channelID:" + std::to_string(channelID) + " fileID:" + std::to_string(fileID);
    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution(command);

    slog_trace("reply returned with: %s", this->reply->element[0]->str);
    fileName = splitDataFileName();

    freeReplyObject(this->reply);

    slog_trace("file name returning from redis is: %s", fileName.c_str());
    return fileName;
}

std::string RedisHandler::splitDataFileName()
{
    /* This function will split the data received 
       from Redis and will return only the file name */

    std::string fileName = this->reply->element[0]->str;

    int pos = fileName.find(':');

    if (pos != std::string::npos)
    {
        fileName = fileName.substr(0, pos);
    }

    return fileName;
}

void RedisHandler::checkExecution(std::string command)
{
    /* Checks if it is possible to execute the command.
       If not then the function will throw an exception */

    if (!this->reply || this->context->err)
    {
        slog_fatal("could not execute command: %s", command.c_str());
        exit(EXIT_FAILURE);
    }
}

std::string RedisHandler::getValue(std::string key)
{
    /* This function will simply get
       a value from Redis by key */

    std::string command = "get " + key;

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution(command);

    slog_trace("reply returned with: %s", this->reply->str);
    std::string value = this->reply->str;
    freeReplyObject(this->reply);

    return value;
}

int RedisHandler::getFileSize(int channelID, int fileID)
{
    /* This function will get the file size
       from given channel ID and file ID */

    std::string command = "hmget channelID:" + std::to_string(channelID) + " fileID:" + std::to_string(fileID);

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution(command);

    slog_trace("reply returned with: %s", this->reply->element[0]->str);
    std::string metaData = this->reply->element[0]->str;
    freeReplyObject(this->reply);

    int fileSize = std::stoi(metaData.substr(metaData.find(':') + 1, metaData.length()));
    return fileSize;
}

void RedisHandler::setChannels(std::vector<std::string> &channels)
{
    /* This function will receive a reference to a 
       vector which will store the results from Redis */

    this->reply = (redisReply *)redisCommand(this->context, "get dirCount");
    checkExecution("get dirCount");

    slog_trace("reply returned with: %s", this->reply->str);
    int dirCount = std::stoi(this->reply->str);
    freeReplyObject(this->reply);

    std::string command = "lrange channels 0 " + std::to_string(dirCount);
    this->reply = (redisReply *)redisCommand(this->context, command.c_str());

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (int i = 0; i < reply->elements; i++)
        {
            channels.push_back(reply->element[i]->str);
        }
    }

    freeReplyObject(this->reply);
}