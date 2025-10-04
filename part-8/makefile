# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Source files
SRCS = algos.cpp client.cpp euler.cpp graphDS.cpp randomGraph.cpp server.cpp union_find.cpp server_run.cpp
HEADERS = algos.hpp euler.hpp graphDS.hpp randomGraph.hpp server.hpp union_find.hpp client.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Target executable for server
SERVER_EXE = server_app

# Target executable for client
CLIENT_EXE = client_app

# Default target: build both
all: $(SERVER_EXE) $(CLIENT_EXE)

# Compile object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link server executable
$(SERVER_EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) server_run.o server.o algos.o euler.o graphDS.o randomGraph.o union_find.o -o $(SERVER_EXE)

# Link client executable
$(CLIENT_EXE): client.o algos.o euler.o graphDS.o randomGraph.o union_find.o
	$(CXX) $(CXXFLAGS) client.o algos.o euler.o graphDS.o randomGraph.o union_find.o -o $(CLIENT_EXE)

# Clean build artifacts
clean:
	rm -f *.o $(SERVER_EXE) $(CLIENT_EXE)

.PHONY: all clean
