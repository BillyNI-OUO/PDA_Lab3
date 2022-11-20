all: Lab3

CXX = g++

LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3 -g

Lab3: main.o block.o
	$(CXX) $(LINKFLAGS) main.o -o Lab3

main.o: main.cpp
	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c


block.o: ./src/block.cpp ./src/block.h
	$(CXX) $(LINKFLAGS) ./src/block.cpp -c

terminal.o: ./src/terminal.cpp ./src/terminal.h
	$(CXX) $(LINKFLAGS) ./src/terminal.cpp -c

vertices.o: ./src/vertices.cpp ./src/vertices.h
	$(CXX) $(LINKFLAGS) ./src/vertices.cpp -c

clean:
	rm -rf *.o *.gch Lab2






