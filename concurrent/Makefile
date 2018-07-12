all: singlethread multithread multiprocess

singlethread: singlethread.cpp
	g++ -o singlethread singlethread.cpp

multithread: multithread.cpp
	g++ -o multithread multithread.cpp -lpthread

multiprocess: multiprocess.cpp
	g++ -o multiprocess multiprocess.cpp

clean:
	rm singlethread multithread multiprocess
