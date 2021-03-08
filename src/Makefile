CC=g++
CFLAGS= -O3 -std=c++2a -Wall -Wextra -Wshadow -Wpedantic -flto

all: list

list:
	mkdir -p ../bin && $(CC) -o ../bin/list list.cpp $(CFLAGS)
install: list
	cp ../bin/list ~/.local/bin/list
clean:
	rm ../bin/list

