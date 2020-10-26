#include "TX.h"

int main()
{
	TXMetaDataSender metaDataSender(IP, META_DATA_PORT);
	metaDataSender.sendMetaData();

	TXDataSender dataSender(IP, DATA_PORT);
	dataSender.startSending();

	return 0;
}