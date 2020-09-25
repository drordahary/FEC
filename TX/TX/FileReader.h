#pragma once

#include <iostream>
#include <fstream>

#define BUFFER_SIZE 512

class FileReader
{
private:
	int filesize;
	std::ifstream file;

public:
	FileReader(const char* filepath);
	~FileReader();
	void readFile(int amountToRead, int position, char buffer[]);
	void closeFile();
	int getFileSize();
};