CC=gcc
CFLAGS=-I.

# Определение объектных файлов
OBJ_SERVER=server.o
OBJ_CLIENT=client.o

# По умолчанию собираем и сервер, и клиента
all: server client

# Цели для компиляции сервера и клиента
server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

# Цель для очистки скомпилированных файлов
clean:
	rm -f $(OBJ_SERVER) $(OBJ_CLIENT) server client

# Специальная цель .PHONY для указания нефайловых целей
.PHONY: all clean
