all:
	make compile

compile: client.c server.c
	gcc -Wall -o client client.c -lpthread
	gcc -Wall -o server server.c -lpthread
	#./server port name np. 
		#./server 8888 serverName
	
	#./client name unix name
		#./client client1 unix severName
	
	#./client name web web_address
		#./client client1 web 127.0.0.1:8888

clean:
	rm client server


