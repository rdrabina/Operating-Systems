all:
	make compile

compile: client.c barber.c
	gcc -o barber barber.c
	gcc -o client client.c

clean:
	rm client barber
