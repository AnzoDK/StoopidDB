release:
	g++ -g3 -std=c++17 testProgram.cpp -o test.out
optimize:
	g++ -O2 -std=c++17 testProgram.cpp -o test.out
x86:
	g++ -m32 -std=c++17 testProgram.cpp -o test.out
clean:
	-rm *.o
