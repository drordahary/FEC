#pragma once

#include <iostream>
#include <fstream>

#define BUFFER_SIZE 512

class FileBuilder
{
private:
	std::ofstream file;

public:
	FileBuilder(const char* filename);
	~FileBuilder();
	void writeToFile(char buffer[]);
	void closeFile();
};