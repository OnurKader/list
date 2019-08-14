CC=g++
CFLAGS= -O3 -std=c++17 -lstdc++fs -Wall -Wextra -pedantic -pedantic-errors

list:
	mkdir -p ../bin && $(CC) -o ../bin/list ./list.cpp $(CFLAGS)

install:
	sudo cp ../bin/list /usr/bin/list -psi

clean:
	rm ../bin/list

