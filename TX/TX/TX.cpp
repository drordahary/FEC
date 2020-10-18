#include "TX.h"

int main()
{
	TXDataSender dataSender(IP, DATA_PORT);
	dataSender.startSending();

	return 0;
}