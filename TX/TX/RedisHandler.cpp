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
    /* The destructor will free all the
       allocated memory automatically */
}

void RedisHandler::connectToRedis()
{
    /* The function will connect to redis and 
       select a specific database to use */

    this->context = redisConnect("127.0.0.1", 6379);

    if (!context)
    {
        throw("Couldn't connect to redis (Check if the server is up)");
    }

    std::string command = "select " + std::to_string(this->databaseID);

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution();

    freeReplyObject(this->reply);
}

int RedisHandler::addToRedis(std::string fileMetaData[])
{
    /* The function will receive a string which
       is the command and will execute it
       The file meta data consist of:
       ID, file name and size.
       The function will also increament
       the currentAmount by 1 for the next file
       return: current file's ID */

    mLock.lock();
    int currentFileID = getLastFileID() + 1;

    this->reply = (redisReply *)redisCommand(this->context, "incr currentAmount");
    checkExecution();

    freeReplyObject(this->reply);
    mLock.unlock();

    std::string command = formatCommand(fileMetaData, currentFileID);

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution();

    freeReplyObject(this->reply);

    return currentFileID;
}

int RedisHandler::getLastFileID()
{
    /* The function will return the last
       (largest) file ID number in redis */

    this->reply = (redisReply *)redisCommand(this->context, "get currentAmount");

    if (!this->reply || this->context->err || this->reply->type != REDIS_REPLY_STRING)
    {
        throw("Couldn't read from redis");
    }

    int lastFileID = atoi(this->reply->str);

    freeReplyObject(this->reply);
    return lastFileID;
}

int RedisHandler::getDirectoryCount()
{
    /* The function will return 
       the amount of channels */

    std::string command = "get dirCount";

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());

    if (!this->reply || this->context->err || this->reply->type != REDIS_REPLY_STRING)
    {
        throw("Couldn't read from redis");
    }

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
    checkExecution();

    std::string channelName = this->reply->str;
    freeReplyObject(this->reply);

    return channelName;
}

std::string RedisHandler::getValue(std::string key)
{
    /* This function will simply get
       a value from Redis by key */

    std::string command = "get " + key;

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution();

    std::string value = this->reply->str;
    freeReplyObject(this->reply);

    return value;
}

std::string RedisHandler::formatCommand(std::string fileMetaData[], int fileID)
{
    /* The function will format the command to be executable.
       The format is: (fileID, fileName, fileSize) */

    std::string command = "hmset fileID:" + std::to_string(fileID) + " fileName " + fileMetaData[0] + " fileSize " + fileMetaData[1];

    return command;
}

void RedisHandler::checkExecution()
{
    /* Checks if it is possible to execute the command.
       If not then the function will throw an exception */

    if (!this->reply || this->context->err)
    {
        throw("Couldn't execute command");
    }
}

std::string RedisHandler::getFileName(int fileID)
{
    /* The function will receive a file ID to
       search for and will return the file name */

    std::string command = "hmget fileID:" + std::to_string(fileID) + " fileName";

    this->reply = (redisReply *)redisCommand(this->context, command.c_str());
    checkExecution();

    std::string fileName = this->reply->element[0]->str;

    freeReplyObject(this->reply);

    return fileName;
}

void RedisHandler::closeConnection()
{
    /* The function will close the 
       socket connected to Redis */

    redisFree(this->context);
}