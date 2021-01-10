main: Socket.o Client.o Server.o Message.o Epoll.o base64.o sha1.o
	g++ -g Socket.o Client.o Server.o Message.o Epoll.o main.cpp -I. -lpthread -ljson -o appServer
Epoll.o: Epoll.cpp
	g++ -c Epoll.cpp
Message.o: Message.cpp
	g++ -c Message.cpp
Server.o: Server.cpp
	g++ -c Server.cpp
Client.o: Client.cpp
	g++ -c Client.cpp
sha1.o: lib/sha1.c
	gcc -c lib/sha1.c
base64.o: lib/base64.c
	gcc -c lib/base64.c
Socket.o: Socket.cpp
	g++ -c Socket.cpp
clean:
	@rm -f appServer *.o
	@echo 'clean'
