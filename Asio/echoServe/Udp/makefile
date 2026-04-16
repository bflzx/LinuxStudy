all:client server
client:client.cpp
	g++ $^ -o $@ -std=c++11 -lboost_system -lboost_thread -lpthread
server:server.cpp
	g++ $^ -o $@ -std=c++11 -lboost_system
.PHONY:clean
clean:
	rm -f client server
