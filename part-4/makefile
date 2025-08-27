CC =clang++
CFLAGS = -Wall -Wextra -g -pg
SRC = main.cpp  ../part-1/graphDS.cpp

all: main

main: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC) -I../part-1 -I../part-2

run: main
	./main

profile: run
	gprof main gmon.out > profile.txt

memcheck:
	valgrind --tool=memcheck --leak-check=full ./main

callgrind:
	valgrind --tool=callgrind ./main

clean:
	rm -f main *.gcov gmon.out profile.txt callgrind.out.*

.PHONY: all run memcheck callgrind clean