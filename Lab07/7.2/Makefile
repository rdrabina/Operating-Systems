all:
	make compile

compile: client.c barber.c
	gcc -o barber barber.c -lrt -lpthread
	gcc -o client client.c -lrt -lpthread

clean:
	rm client barber
