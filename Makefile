#g++ -o example olcExampleProgram.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14 
all: chess

chess: chess.o
	g++ -o chess Chess.o -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14

chess.o: Chess.cpp
	g++ -c Chess.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++14

clean:
	rm *.o
