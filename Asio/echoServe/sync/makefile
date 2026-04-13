all:client server
client:client.cpp
	g++ $^ -o $@ -lboost_system -lboost_thread -lpthread
server:server.cpp
	g++ $^ -o $@ -lboost_system -lboost_thread -lpthread
.PHONY:clean
clean:
	rm -f client server