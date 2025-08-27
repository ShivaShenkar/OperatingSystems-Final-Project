CC =clang++
CFLAGS = -Wall -Wextra -g
SRC = main.cpp ../part-1/graphDS.cpp

all: main

main: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC) -I../part-2 -I ../part-1

clean:
	rm -f main

.PHONY: all clean