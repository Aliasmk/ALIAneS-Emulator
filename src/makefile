

all:
	g++ *.hpp
	g++ *.cpp -o ../bin/System -framework SDL2

run:
	../bin/System

debug:
	gdb-apple ../bin/System

test:
	g++ ../res/testcases.cpp -o ../bin/Test

runtest:
	../bin/Test
