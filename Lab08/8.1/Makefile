all:
	make compile

compile: main.c 
	gcc -Wall -o main main.c -lpthread -lm

test: main.c
	gcc -Wall -o main main.c -lpthread -lm
	echo Filter size: 40 >> Times.txt
	./main 1 image.ascii.pgm filter image2.ascii.pgm >> Times.txt
	./main 2 image.ascii.pgm filter image2.ascii.pgm >> Times.txt
	./main 4 image.ascii.pgm filter image2.ascii.pgm >> Times.txt
	./main 8 image.ascii.pgm filter image2.ascii.pgm >> Times.txt
	echo Filter size: 25 >> Times.txt
	./main 1 image.ascii.pgm filter2 image2.ascii.pgm >> Times.txt
	./main 2 image.ascii.pgm filter2 image2.ascii.pgm >> Times.txt
	./main 4 image.ascii.pgm filter2 image2.ascii.pgm >> Times.txt
	./main 8 image.ascii.pgm filter2 image2.ascii.pgm >> Times.txt

clean:
	rm main image2.ascii.pgm


