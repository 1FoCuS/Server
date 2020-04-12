CC = gcc
SRC = ./src/
BIN = ./bin/
BUILD = ./build/
OBJS_S = server.o network.o
OBJS_C = client.o network.o

all: server client

server: $(OBJS_S)
	$(CC) $(addprefix $(BIN), $(OBJS_S)) -o $(BUILD)server

client: $(OBJS_C)
	$(CC) $(addprefix $(BIN), $(OBJS_C)) -o $(BUILD)client

%.o: $(SRC)%.c
	echo "CC " $<
	$(CC) -o $(BIN)$@ -c $<

clean:
	rm  $(BIN)*.o