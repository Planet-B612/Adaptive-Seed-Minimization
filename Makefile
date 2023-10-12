all: trim pwic

pwic: src/*.cpp src/*.h
	g++ src/pw_ic.cpp -Wall -std=c++11 -O3 src/sfmt/SFMT.c -o pwic


trim: src/*.cpp src/*.h
		g++ src/asm.cpp -Wall -std=c++11 -O3 src/sfmt/SFMT.c -o trim
