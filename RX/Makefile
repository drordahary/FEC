install: install-hiredis install-slog install-service
	/usr/bin/g++ -g RX/RX.cpp RX/Deserializer.cpp RX/FileBuilder.cpp RX/RXReceiver.cpp RX/RXDataReceiver.cpp RX/RedisHandler.cpp RX/RXMetaDataReceiver.cpp RX/HierarchyBuilder.cpp RX/Settings.cpp RX/FileTracker.cpp RX/FileMonitor.cpp -o RX/RX -lhiredis -lslog -lpthread

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

install-service:
	sudo chmod +x $(CURDIR)/RX;
	if [ ! -f /etc/systemd/system/RX.service ]; then \
		sudo touch /etc/systemd/system/RX.service; \
		sudo bash -c 'echo "[Service]" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "ExecStart=$(CURDIR)/RX/RX" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "Restart=always" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "WorkingDirectory=$(CURDIR)/RX" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "User=${SUDO_USER}" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "Group=${SUDO_USER}" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "[Install]" >> /etc/systemd/system/RX.service'; \
		sudo bash -c 'echo "WantedBy=multi-user.target" >> /etc/systemd/system/RX.service'; \
	fi;