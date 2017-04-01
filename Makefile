all: rockit

rockit: rockit.o
	g++  -o -std=c++11 bin/rockit obj/rockit.o -lSDL2 -lSDL2_image 

rockit.o: src/rockit.cpp
	g++ -c -std=c++11 -o obj/rockit.o  src/rockit.cpp -lSDL2 -lSDL2_image 

clean:
	rm obj/*.o  bin/rockit

install: 
	cp bin/rockit /usr/local/bin
	
	

