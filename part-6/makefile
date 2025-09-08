CC =clang++
CFLAGS = -Wall -Wextra -g 
SERSRC = server.cpp  ../part-1/graphDS.cpp
CLIENTSRC = client.cpp

all: client server

server: $(SERSRC)
	$(CC) $(CFLAGS) -o server $(SERSRC) -I../part-1 -I../part-2 -I../part-4

client: $(CLIENTSRC)
	$(CC) $(CFLAGS) -o client $(CLIENTSRC)

clean:
	rm -f client server 

.PHONY: all server client clean