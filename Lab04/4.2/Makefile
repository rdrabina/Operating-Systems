all:	main.o child.o
	./main 10 1
	./main 10 10

main.o: main.c
	gcc -Wall main.c -o main

child.o: child.c
	gcc -Wall child.c -o child
clean:
	rm main child
