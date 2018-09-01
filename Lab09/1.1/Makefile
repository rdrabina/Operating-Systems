all:
	make compile

compile: main.c 
	gcc -Wall -o main main.c -lpthread 

test: main.c
	gcc -Wall -o main main.c -lpthread
	./main configuration
clean:
	rm main 


