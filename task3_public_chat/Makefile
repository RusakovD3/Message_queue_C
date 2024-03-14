SERVER_BIN = server
CLIENT_BIN = client1
CLIENT_BIN2 = client2

CC = gcc

CFLAGS =  -Wall -Werror -Wextra -lrt 

SERVER_SRC = server.c
CLIENT_SRC = client.c

all: $(SERVER_BIN) $(CLIENT_BIN) $(CLIENT_BIN2)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

$(CLIENT_BIN2): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN) $(CLIENT_BIN2)

.PHONY: all clean
