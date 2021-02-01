CPPFLAGS=-Wall -g
OBJECTS=main.o game.o
%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<
tetris: $(OBJECTS)
	g++ $(OBJECTS) -o checker -g -lglut -lGL
main.o: main.cpp game.hpp
game.o: game.cpp game.hpp
