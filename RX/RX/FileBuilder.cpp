#include "FileBuilder.h"

FileBuilder::FileBuilder() : file(NULL)
{
	/* This constructor is used for specific in
	   class use where the object cannot 
	   be initialized immediately */
}

FileBuilder::~FileBuilder()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
}

void FileBuilder::setFile(std::string filename)
{
	/* The function will initialize ofstream.
	   If the file doesn't exists it'll create one */

	this->file.open(filename.c_str(), std::ios_base::app);

	if (!this->file.is_open())
	{
		throw("Error while trying to opening / creating the file");
	}
}

void FileBuilder::writeToFile(char buffer[])
{
	/* The function will receive a buffer as a 
	   parameter and will append it to the file */

	if (this->file.is_open())
	{
		this->file << buffer;
		this->file.flush();
	}
}

void FileBuilder::closeFile()
{
	/* This function will be called if the program
	   crashes or the program finished */

	this->file.close();
}