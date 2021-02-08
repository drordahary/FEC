#pragma once

#include <iostream>
#include <fstream>
#include <limits>

#define BUFFER_SIZE 512

class FileReader
{
private:
	int filesize;
	std::ifstream file;

public:
	FileReader();
	~FileReader();

	void setFile(const char *filepath);

	void readFile(int amountToRead, int position, char buffer[]);
	void closeFile();
	int getFileSize();
};