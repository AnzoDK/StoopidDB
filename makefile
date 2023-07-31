release:
	g++ -Wall -g3 -std=c++17 testProgram.cpp -o test.out
optimize:
	g++ -Wall -O2 -std=c++17 testProgram.cpp -o test.out
x86:
	g++ -Wall -m32 -std=c++17 testProgram.cpp -o test.out
clean:
	-rm *.o
