SRC_DIR:=src

all: $(patsubst ./$(SRC_DIR)/%.c,%,$(shell find ./$(SRC_DIR) -name '*.c'))

benchmark-server: $(SRC_DIR)/benchmark-server.c
	$(CC) -pthread -o $@ $<

netlink-listener: $(SRC_DIR)/netlink-listener.c
	$(CC) -I/usr/lib/libnl -o $@ $<

.PHONY: all
