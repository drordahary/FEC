#include "RedisHandler.h"

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
        throw("Couldn't connect to redis (Check if the server is up)");
    }

    std::string command = "select " + this->databaseID;

    this->reply = (redisReply*)redisCommand(this->context, command.c_str());
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

    int currentFileID = getLastFileID() + 1;
    std::string command = formatCommand(fileMetaData, currentFileID);

    this->reply = (redisReply*)redisCommand(this->context, command.c_str());
    checkExecution();

    freeReplyObject(this->reply);

    this->reply = (redisReply*)redisCommand(this->context, "incr currentAmount");
    checkExecution();

    freeReplyObject(this->reply);
    
    return currentFileID;
}

int RedisHandler::getLastFileID()
{
    /* The function will return the last
       (largest) file ID number in redis */

    this->reply = (redisReply*)redisCommand(this->context, "get currentAmount");

    if (!this->reply || this->context->err || this->reply->type != REDIS_REPLY_STRING)
    {
        throw("Couldn't read from redis");
    }

    int lastFileID = atoi(this->reply->str);

    freeReplyObject(this->reply);
    return lastFileID;
}

std::string RedisHandler::formatCommand(std::string fileMetaData[], int fileID)
{
    /* The function will format the command to be executable.
       The format is: (fileID, fileName, fileSize) */

    std::string command = "hmset fileID:" + std::to_string(fileID)
                        + " fileName " + fileMetaData[0]
                        + " fileSize " + fileMetaData[1];

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