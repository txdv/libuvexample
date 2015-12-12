LIBUV=/home/bentkus/Projects/c/libuv/.libs/

all: client server

client: client.c
	gcc client.c -I ../libuv/include/ -o client -luv -L$(LIBUV)


server: server.c
	gcc server.c -I ../libuv/include/ -o server -luv -L$(LIBUV)
