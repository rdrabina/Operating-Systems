CC = gcc

all: static shared clean

static: 
	$(CC) -c -Wall libstatic.c libdynamic.c libusefulfunctions.c
	ar rcs library.a libstatic.o libdynamic.o libusefulfunctions.o

shared:
	$(CC) -c -Wall -fPIC libstatic.c libdynamic.c libusefulfunctions.c
	$(CC) -shared -fPIC -o library.so libstatic.o libdynamic.o libusefulfunctions.o


clean: 
	rm -f *.o *.so *.a
