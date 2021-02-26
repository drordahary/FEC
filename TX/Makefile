install: install-hiredis install-slog install-service
	/usr/bin/g++ -g TX/TX.cpp TX/FileReader.cpp TX/Serializer.cpp TX/RedisHandler.cpp TX/DirectoryReader.cpp TX/TXSender.cpp TX/TXDataSender.cpp TX/TXMetaDataSender.cpp TX/HierarchyBuilder.cpp TX/ThreadPool.cpp TX/Settings.cpp -o TX/TX -lhiredis -lslog -lpthread

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
	sudo chmod +x $(CURDIR)/TX;
	if [ ! -f /etc/systemd/system/TX.service ]; then \
		sudo touch /etc/systemd/system/TX.service; \
		sudo bash -c 'echo "[Service]" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "ExecStart=$(CURDIR)/TX/TX" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "Restart=always" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "WorkingDirectory=$(CURDIR)/TX" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "User=${SUDO_USER}" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "Group=${SUDO_USER}" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "[Install]" >> /etc/systemd/system/TX.service'; \
		sudo bash -c 'echo "WantedBy=multi-user.target" >> /etc/systemd/system/TX.service'; \
	fi;