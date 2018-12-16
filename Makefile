all: 	morphological_ops.cpp	driver.cpp
	# g++ $(pkg-config --cflags --libs opencv) pattern_matching.cpp -o pattern_matching.exe --std=c++14
	# g++ $(pkg-config --cflags --libs opencv) extract.cpp -o extract.exe --std=c++14
	# g++ $(pkg-config --cflags --libs opencv) deskew.cpp -o deskew.exe --std=c++14

	# g++ $(pkg-config --cflags --libs opencv) driver.cpp -o driver.exe --std=c++14
	g++ $(pkg-config --cflags --libs opencv) driver.cpp morphological_ops.cpp -o driver.exe --std=c++14
driver.cpp:
	g++ $(pkg-config --cflags --libs opencv) -c driver.cpp --std=c++14
morphological_ops.cpp:
	g++ $(pkg-config --cflags --libs opencv) -c morphological_ops.cpp --std=c++14
clean:
	# rm -rf *.o pattern_matching.exe extract.exe	deskew.exe ./*.jpg
	rm -rf *.o driver.exe ./*.jpg