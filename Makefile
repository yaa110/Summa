SRC_DIR:=src

benchmark-server: $(SRC_DIR)/benchmark-server.c
	$(CC) -pthread -o $@ $<

netlink-listener: $(SRC_DIR)/netlink-listener.c
	$(CC) -I/usr/lib/libnl -o $@ $<
