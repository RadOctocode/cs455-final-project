all:	
	g++ $(pkg-config --cflags --libs opencv) pattern_matching.cpp -o pattern_matching.exe --std=c++11
	g++ $(pkg-config --cflags --libs opencv) extract.cpp -o extract.exe --std=c++11

clean:
	rm -rf *.o pattern_matching.exe extract.exe