#include "ThreadPool.h"

ThreadPool::ThreadPool(std::string workingChannel, std::vector<int> portRange)
{
    /* The constructor will initialize all 
       the threads to start waiting */

    this->poolTerminated = false;

    for (auto port = portRange.begin(); port != portRange.end(); port++)
    {
        this->pool.push_back(std::thread(&ThreadPool::waitForJob, this, *port));
    }
}

ThreadPool::~ThreadPool()
{
    /* The destructor will automatically 
       deallocate the memory consumed */
}

void ThreadPool::waitForJob(int port)
{
    /* The thread will wait on an 
       infinite loop until terminated */

    std::string path;

    while (true)
    {
        // Waiting for a job (new file available)

        std::unique_lock<std::mutex> lock(this->queueMutex);
        this->condition.wait(lock, [this] { return !availableFiles.empty() || poolTerminated; });

        // Taking the path

        path = this->availableFiles.front();
        this->availableFiles.pop();

        // Unlocking for the next job

        lock.unlock();

        // Start to work

        startJob(path, port);
    }
}

void ThreadPool::startJob(std::string data, int port)
{
    /* This function will call the 
       TX to start sending the file */

    TXDataSender sender(LOCAL_IP, port, this->workingChannel);
    FileReader reader;

    std::string splittedData[2];

    char delimeter = ',';
    size_t position = data.find(delimeter);
    
    splittedData[0] = data.substr(0, position);
    data.erase(0, position + 1);
    splittedData[1] = data;

    reader.setFile(splittedData[0].c_str());
    int size = reader.getFileSize();
    reader.closeFile();

    int fileID = std::stoi(splittedData[1]);

    sender.preparePackets(size, fileID, splittedData[0]);
}

void ThreadPool::addJob(std::string data)
{
    /* The function will add a 
       new job to the queue */

    std::unique_lock<std::mutex> lock(this->queueMutex);

    this->availableFiles.push(data);

    lock.unlock();
    this->condition.notify_one();
}

void ThreadPool::shutdown()
{
    /* This function will join all the
       threads to terminate the pool */

    std::unique_lock<std::mutex> lock(this->queueMutex);
    poolTerminated = true;

    //this->condition.notify_all();

    for (std::thread &currentThread : this->pool)
    {
        currentThread.join();
    }

    this->pool.clear();
}