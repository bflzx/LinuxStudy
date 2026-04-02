all:client server
client:client.cpp
	g++ $^ -o $@ -lboost_system -pthread
server:server.cpp
	g++ $^ -o $@ -lboost_system -pthread

.PHONY:clean
clean:
	rm -f client server