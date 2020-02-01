CC=g++
CFLAGS= -O3 -std=c++17 -lstdc++fs -Wall -Wextra -pedantic -pedantic-errors -g

all: list

list:
	mkdir -p ../bin && $(CC) -o ../../bin/list list.cpp $(CFLAGS)
install:
	sudo ln -sf /home/beronthecolossus/Peki/C++/Projects/bin/list /usr/bin/list
clean:
	rm ../bin/list

