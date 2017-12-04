SRC_DIR=src

benchmark-server: $(SRC_DIR)/benchmark-server.c
	$(CC) -pthread -o $@ $<
