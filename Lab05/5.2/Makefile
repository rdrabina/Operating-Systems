all: master.o slave.o

master.o: master.c
	gcc -Wall master.c -o master

slave.o: slave.c
	gcc -Wall slave.c -o slave

clean: 
	rm -r master slave
