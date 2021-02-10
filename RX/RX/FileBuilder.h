#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <slog.h>

#define FILES_PATH "./Files"

class FileBuilder
{
private:
	int fd;

public:
	FileBuilder();
	~FileBuilder();

	void setFile(std::string filename, const char option);

	void writeToFile(char buffer[], int bufferLength, int offset);
	void appendToFile(const char *buffer, int bufferLength);
	void closeFile();

	void flushBuffer();
};