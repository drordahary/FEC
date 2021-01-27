#pragma once

#include <iostream>
#include <fstream>

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