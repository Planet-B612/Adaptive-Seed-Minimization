all: trim pwic test

pwic: *.cpp *.h
	g++ pw_ic.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o pwic


trim: *.cpp *.h
		g++ asm.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o trim

test: *.cpp *.h
		g++ test.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o test
