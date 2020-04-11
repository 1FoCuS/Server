CC = gcc
OBJ = server.o network.o

server: $(OBJ)
	$(CC) $(OBJ) -o server

%.o: %.c
	echo "CC " $<
	$(CC) -o $@ -c $<

clean:
	echo "rm OBJ"
	rm *.o