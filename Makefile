all:	
	g++ $(pkg-config --cflags --libs opencv) pattern_matching.cpp -o pattern_matching.exe --std=c++14
	g++ $(pkg-config --cflags --libs opencv) extract.cpp -o extract.exe --std=c++14
	g++ $(pkg-config --cflags --libs opencv) deskew.cpp -o deskew.exe --std=c++14

clean:
	rm -rf *.o pattern_matching.exe extract.exe	deskew.exe