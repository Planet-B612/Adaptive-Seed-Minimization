all: trim asm

pwic: *.cpp *.h
	g++ pw_ic.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o pwic


trim: *.cpp *.h
		g++ asm.cpp -Wall -g -std=c++11 -O0 sfmt/SFMT.c -o trim

asm: *.cpp *.h
		g++ asm.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o asm

test: *.cpp *.h
		g++ test.cpp -Wall -std=c++11 -O3 sfmt/SFMT.c -o test
