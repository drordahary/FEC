#include "FileReader.h"

FileReader::FileReader() : file(NULL)
{
	/* This constructor is used for specific in class use 
	   where the object cannot be initialized immediately */
}

FileReader::~FileReader()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void FileReader::setFile(const char *filepath)
{
	/* The function will initialize ifstream
	   and will set the file size */

	this->file.open(filepath, std::ios::binary);

	if (!this->file.is_open())
	{
		throw("Error while trying to open the file. check if the file exists first");
	}

	this->file.seekg(0, std::ios::end);

	this->filesize = this->file.tellg();

	this->file.seekg(0, std::ios::beg);
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