#include "FileBuilder.h"

FileBuilder::FileBuilder(const char* filename) : file(filename, std::ios_base::app)
{
	/* The constructor will use initializer list
	   to create the file or open it if it exists
	   and also using the append mode 
	   to prevent overwriting */
}

FileBuilder::~FileBuilder()
{
	/* The destructor will delete automatically
	   all the allocated memory of the object */
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