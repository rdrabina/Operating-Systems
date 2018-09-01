all:
	make compile

compile: client.c server.c
	gcc -Wall -o server server.c -lrt
	gcc -Wall -o client client.c -lrt

clean:
	rm client server
