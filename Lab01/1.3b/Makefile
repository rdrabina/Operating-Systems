CC = gcc
optim = 0
	

test: static shared
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -O0
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -O0
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -O0
	echo with optymalization 0 >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -O3
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -O3
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -O3
	echo with optymalization 3 >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -Os
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -Os
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -Os
	echo with optymalization s >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt

opt0: static shared
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -O0
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -O0
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -O0
	echo with optymalization 0 >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt

opt3: static shared
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -O3
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -O3
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -O3
	echo with optymalization 3 >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt

opts: static shared
	$(CC) -Wall -std=c99 -static static.c libstatic.c libusefulfunctions.c -L. -o staticTest -Os
	$(CC) -Wall -std=c99 -L. -o sharedTest static.c libstatic.c libusefulfunctions.c -Os
	$(CC) -Wall -std=c99 dynamic.c libdynamic.c libusefulfunctions.c -o dynamicTest -ldl -Os
	echo with optymalization s >> results3b.txt
	echo test_static >> results3b.txt
	./staticTest 1000 30 static a b c d >> results3b.txt
	echo test_shared >> results3b.txt
	./sharedTest 1000 30 static a b c d >> results3b.txt
	echo test_dynamic >> results3b.txt
	./dynamicTest 5 30 dynamic a b c d >> results3b.txt

static: 
	$(CC) -c -Wall libstatic.c libdynamic.c libusefulfunctions.c
	ar rcs library.a libstatic.o libdynamic.o libusefulfunctions.o

shared:
	$(CC) -c -Wall -fPIC libstatic.c libdynamic.c libusefulfunctions.c
	$(CC) -shared -fPIC -o library.so libstatic.o libdynamic.o libusefulfunctions.o

clean: 
	rm -f *.o *.so *.a
