seconds = 1
megabytes = 50

all: main.o
	./main example.txt $(seconds) $(megabytes)

main.o: main.c
	gcc -Wall main.c -o main

clean: 
	rm -r directory file.c main
