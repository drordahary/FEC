#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>

class FileBuilder
{
private:
	FILE *file;

public:
	FileBuilder();
	~FileBuilder();

	void createNewFile(int size, std::string fileName);
	void setFile(std::string filename);

	void writeToFile(char buffer[], int bufferLength, int offset);
	void closeFile();
};