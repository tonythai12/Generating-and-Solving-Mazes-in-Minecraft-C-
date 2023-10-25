.default: all

all: mazeRunner

clean:
	rm -f mazeRunner *.o

mazeRunner: Maze.o Agent.o mazeRunner.o
	g++ -I/usr/local/include/ -Wall -Werror -std=c++17 -g -O -o $@ $^ -lmcpp

%.o: %.cpp
	g++ -I/usr/local/include/ -Wall -Werror -std=c++17 -g -O -c $^