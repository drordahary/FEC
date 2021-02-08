#include "FileMonitor.h"

FileMonitor::FileMonitor(int bufferSize)
{
    /* The constructor will initialize
       the field bufferSize */

    this->bufferSize = bufferSize;
}

FileMonitor::~FileMonitor()
{
    /* The destructor will deallocate 
       all of the object's memory */
}

int FileMonitor::calculateOffset(int fileSize, int packetID, int packetSize)
{
    /* The function will calculate the offset 
       which we need to start writing from */

    int fullBuffersCount = fileSize / (this->bufferSize - (_HEX_LENGTH * 3));
    int offset = 0;

    if (packetID == 0)
    {
        return 0;
    }

    else if (packetSize < (this->bufferSize - (_HEX_LENGTH * 3)))
    {
        offset = fullBuffersCount * (this->bufferSize - (_HEX_LENGTH * 3));
    }

    else
    {
        offset = packetID * packetSize;
    }

    return offset;
}