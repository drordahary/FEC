install: install-hiredis install-slog
	/usr/bin/g++ -g RX/RX/RX.cpp RX/RX/Deserializer.cpp RX/RX/FileBuilder.cpp RX/RX/RXReceiver.cpp RX/RX/RXDataReceiver.cpp RX/RX/RedisHandler.cpp RX/RX/RXMetaDataReceiver.cpp RX/RX/HierarchyBuilder.cpp RX/RX/Settings.cpp RX/RX/FileTracker.cpp RX/RX/FileMonitor.cpp -o RX/RX/RX -lhiredis -lslog -lpthread
	/usr/bin/g++ -g TX/TX/TX.cpp TX/TX/FileReader.cpp TX/TX/Serializer.cpp TX/TX/RedisHandler.cpp TX/TX/DirectoryReader.cpp TX/TX/TXSender.cpp TX/TX/TXDataSender.cpp TX/TX/TXMetaDataSender.cpp TX/TX/HierarchyBuilder.cpp TX/TX/ThreadPool.cpp TX/TX/Settings.cpp -o TX/TX/TX -lhiredis -lslog -lpthread

install-hiredis:
	if [ -d "/usr/local/lib/hiredis" ]; then \
		echo "hiredis exists"; \
	else \
		git clone https://github.com/redis/hiredis.git /usr/local/lib/hiredis/; \
	fi

	$(MAKE) -C /usr/local/lib/hiredis/
	sudo $(MAKE) -C /usr/local/lib/hiredis/ install

install-slog:
	cd /usr/local/lib
	if [ -d "/usr/local/lib/slog" ]; then \
		echo "slog exists"; \
	else \
		git clone https://github.com/kala13x/slog.git /usr/local/lib/slog/; \
	fi

	$(MAKE) -C /usr/local/lib/slog/
	sudo $(MAKE) -C /usr/local/lib/slog/ install