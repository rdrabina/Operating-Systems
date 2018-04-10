seconds = 1
megabytes = 30

all: main.o memoryTest.o timeTest.o
	
	./main example1.txt $(seconds) $(megabytes)
	./main example2.txt $(seconds) $(megabytes)
	

main.o: main.c
	gcc -Wall main.c -o main

memoryTest.o: memoryTest.c
	gcc -Wall memoryTest.c -o memory

timeTest.o: timeTest.c
	gcc -Wall timeTest.c -o time

clean: 
	rm -r directory main memory time
