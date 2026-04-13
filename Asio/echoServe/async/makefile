all:client server
client:client.cpp
	g++ $^ -o $@ -lboost_system -lpthread
server:server.cpp
	g++ $^ -o $@ -lboost_system -lpthread
.PHONY:clean
clean:
	rm -f client server