#pragma once

#include <iostream>
#include <fstream>

#define BUFFER_SIZE 512

class FileBuilder
{
private:
	std::ofstream file;

public:
	FileBuilder();
	~FileBuilder();

	void setFile(std::string filename);

	void writeToFile(char buffer[]);
	void closeFile();
};