#include "FileBuilder.h"

FileBuilder::FileBuilder()
{
	/* This constructor is used for specific in
	   class use where the object cannot 
	   be initialized immediately */

	this->fd = -1;
}

FileBuilder::~FileBuilder()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void FileBuilder::setFile(std::string filename, const char option)
{
	/* The function will initialize ofstream.
	   If the file doesn't exists it'll create one */

	if (option == 'a')
	{
		this->fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0700);
	}

	else if (option == 'w')
	{
		this->fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0700);
	}

	if (this->fd == -1)
	{
		slog_fatal("could not open file: %s", filename.c_str());
		exit(EXIT_FAILURE);
	}
}

void FileBuilder::writeToFile(char buffer[], int bufferLength, int offset)
{
	/* The function will receive a buffer as a 
	   parameter and will append it to the file */

	if (this->fd != -1)
	{
		lseek(this->fd, offset, SEEK_SET);
		write(this->fd, buffer, bufferLength);
	}
}

void FileBuilder::appendToFile(const char *buffer, int bufferLength)
{
	/* This function will append the buffer data to EOF
	   Will be mainly used for untracked files */

	if (this->fd != -1)
	{
		write(this->fd, buffer, bufferLength);
	}
}

void FileBuilder::closeFile()
{
	/* This function will be called if the program
	   crashes or the program finished */

	if (this->fd != -1)
	{
		close(this->fd);
	}
}

void FileBuilder::flushBuffer()
{
	/* The function will make sure the buffer is
	   flushed so it is being written to the file */

	fsync(this->fd);
}