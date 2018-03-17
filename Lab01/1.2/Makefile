CC = gcc
	

test: static shared  
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c 
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl
	echo without optymalization >> raport2.txt
	echo test_static >> raport2.txt
	./staticTest 1000 30 static a b c d >> raport2.txt
	echo test_shared >> raport2.txt
	./sharedTest 1000 30 static a b c d >> raport2.txt
	echo test_dynamic >> raport2.txt
	./dynamicTest 5 30 dynamic a b c d >> raport2.txt

static: 
	$(CC) -c -Wall libstatic.c libdynamic.c libusefulfunctions.c
	ar rcs library.a libstatic.o libdynamic.o libusefulfunctions.o

shared:
	$(CC) -c -Wall -fPIC libstatic.c libdynamic.c libusefulfunctions.c
	$(CC) -shared -fPIC -o library.so libstatic.o libdynamic.o libusefulfunctions.o
	


clean: 
	rm -f *.o *.so *.a
