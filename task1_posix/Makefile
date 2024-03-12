SERVER_BIN = server
CLIENT_BIN = client

CC = gcc
CFLAGS = -Wall -Werror -Wextra -lrt

SERVER_SRC = server.c
CLIENT_SRC = client.c

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

.PHONY: all clean
