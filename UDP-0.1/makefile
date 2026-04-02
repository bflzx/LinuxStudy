.PHONY:all
all:udpClient udpServer
udpClient:udpClient.cc
	g++ -o $@ $^ -std=c++11 -lpthread
udpServer:udpServer.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f udpClient udpServer