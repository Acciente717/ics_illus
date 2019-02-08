all:

linux: proxy.cpp
	g++ -std=c++17 -O2 -o proxy proxy.cpp -l pthread -l boost_system -l boost_context -l boost_coroutine

mac: proxy.cpp
	g++ -std=c++17 -O2 -o proxy proxy.cpp -l pthread -l boost_system-mt -l boost_context-mt -l boost_coroutine-mt

clean:
	rm proxy
