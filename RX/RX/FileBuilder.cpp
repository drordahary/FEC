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

void FileBuilder::createNewFile(int size, std::string fileName)
{
	/* This function will create a sparse file, 
	   enabling us to write to it from ant position */

	this->file = fopen(fileName.c_str(), "wb");
	fseek(file, size - 1, SEEK_SET);
	fputc('\0', file);
	fclose(this->file);
}

void FileBuilder::setFile(std::string filename, const char *option)
{
	/* The function will initialize ofstream.
	   If the file doesn't exists it'll create one */

	this->file = fopen(filename.c_str(), option);

	if (this->file == NULL)
	{
		printf("Error while trying to opening / creating the file");
		exit(EXIT_FAILURE);
	}
}

void FileBuilder::writeToFile(char buffer[], int bufferLength, int offset)
{
	/* The function will receive a buffer as a 
	   parameter and will append it to the file */

	if (this->file != NULL)
	{
		fseek(this->file, offset, SEEK_SET);
		fwrite(buffer, sizeof(char), bufferLength, this->file);
	}
}

void FileBuilder::appendToFile(const char *buffer)
{
	/* This function will append the buffer data to EOF
	   Will be mainly used for untracked files */

	if (this->file != NULL)
	{
		fprintf(this->file, "%s", buffer);
	}
}

void FileBuilder::closeFile()
{
	/* This function will be called if the program
	   crashes or the program finished */

	if (this->file != NULL)
	{
		fclose(this->file);
	}
}