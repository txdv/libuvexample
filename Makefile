
client: client.c
	gcc client.c -I ../libuv/include/ -o client -luv -L/home/bentkus/Projects/c/libuv/.libs/


server: server.c
	gcc server.c -I ../libuv/include/ -o server -luv -L/home/bentkus/Projects/c/libuv/.libs/
