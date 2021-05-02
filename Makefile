#g++ -o example olcExampleProgram.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14 
all: example

example: example.o
	g++ -o example Chess.o -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14

example.o: Chess.cpp
	g++ -c Chess.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14

clean:
	rm *.o
