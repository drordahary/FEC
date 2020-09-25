#include "FileReader.h"

FileReader::FileReader(const char* filepath) : file(filepath, std::ios::binary)
{
	/* The constructor will use initializer
	   list to open the file it if it exists */

	if (!this->file.is_open())
	{
		throw ("Error while trying to open the file. check if the file exists first");
	}

	this->file.seekg(0, std::ios::end);

	this->filesize = this->file.tellg();

	this->file.seekg(0, std::ios::beg);
}

FileReader::~FileReader()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void FileReader::readFile(int amountToRead, int position, char buffer[])
{
	/* The function will read a specific amount
	  of bytes / characters into a buffer */

	// Note: amountToRead always smaller or equals to BUFFER_SIZE

	this->file.seekg(position);
	this->file.read(buffer, amountToRead);
}

void FileReader::closeFile()
{
	/* This function will be called if the program
	   crashes or the program finished */

	this->file.close();
}

int FileReader::getFileSize()
{
	/* Returns file's size */

	return this->filesize;
}