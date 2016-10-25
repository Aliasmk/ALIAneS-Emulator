CC=g++
CFLAGS=-framework SDL2 -I. -g
CFILE=Cartridge.cpp CPU.cpp PPU.cpp SDLrender.cpp Shell.cpp System.cpp
HEAD=Cartridge.hpp CPU.hpp PPU.hpp SDLrender.hpp Shell.hpp System.hpp
OBJ=Cartridge.o CPU.o PPU.o SDLrender.o Shell.o System.o




all: 	
	$(CC) -c $(CFILE) -g
	$(CC) -o ../bin/ALIAneS $(OBJ) $(CFLAGS)
	rm *.o
	
debug:
	g++ *.hpp -g
	g++ *.cpp $(CFLAGS) -o ../bin/ALIAneS

run:
	../bin/ALIAneS

gdb:
	gdb-apple ../bin/ALIAneS

test:
	g++ ../res/testcases.cpp -o ../bin/Test

runtest:
	../bin/Test
