install: install-hiredis install-slog install-services
	sudo chmod -R 777 RX/RX/Files
	sudo chmod -R 777 RX/RX/Untracked
	sudo chmod -R 777 RX/RX/Logs
	sudo chmod -R 777 TX/TX/Archive
	sudo chmod -R 777 TX/TX/ToSend
	sudo chmod -R 777 TX/TX/Logs
	/usr/bin/g++ -g RX/RX/RX.cpp RX/RX/Deserializer.cpp RX/RX/FileBuilder.cpp RX/RX/RXReceiver.cpp RX/RX/RXDataReceiver.cpp RX/RX/RedisHandler.cpp RX/RX/RXMetaDataReceiver.cpp RX/RX/HierarchyBuilder.cpp RX/RX/Settings.cpp RX/RX/FileTracker.cpp RX/RX/FileMonitor.cpp -o RX/RX/RX -lhiredis -lslog -lpthread
	/usr/bin/g++ -g TX/TX/TX.cpp TX/TX/FileReader.cpp TX/TX/Serializer.cpp TX/TX/RedisHandler.cpp TX/TX/DirectoryReader.cpp TX/TX/TXSender.cpp TX/TX/TXDataSender.cpp TX/TX/TXMetaDataSender.cpp TX/TX/HierarchyBuilder.cpp TX/TX/ThreadPool.cpp TX/TX/Settings.cpp -o TX/TX/TX -lhiredis -lslog -lpthread

install-hiredis:
	if [ -d "/usr/local/lib/hiredis" ]; then \
		echo "hiredis exists"; \
	else \
		git clone https://github.com/redis/hiredis.git /usr/local/lib/hiredis/; \
		$(MAKE) -C /usr/local/lib/hiredis/; \
		sudo $(MAKE) -C /usr/local/lib/hiredis/ install; \
	fi

install-slog:
	if [ -d "/usr/local/lib/slog" ]; then \
		echo "slog exists"; \
	else \
		git clone https://github.com/kala13x/slog.git /usr/local/lib/slog/; \
		$(MAKE) -C /usr/local/lib/slog/; \
		sudo $(MAKE) -C /usr/local/lib/slog/ install; \
	fi

install-services:
	sudo chmod +x $(CURDIR)/RX/RX/RX;
	if [ ! -f /etc/systemd/system/RX.service ]; then \
		sudo touch /etc/systemd/system/RX.service; \
		sudo bash -c 'echo "[Service]" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "ExecStart=$(CURDIR)/RX/RX/RX" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "Restart=always" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "WorkingDirectory=$(CURDIR)/RX/RX" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "User=${SUDO_USER}" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "Group=${SUDO_USER}" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "[Install]" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "WantedBy=multi-user.target" >> /etc/systemd/system/RX.service'; \
		cd /etc/systemd/system/ && sudo systemctl start RX.service; \
		cd /etc/systemd/system/ && sudo systemctl enable RX.service; \
	fi;