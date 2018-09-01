#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define BUFFER 1000

int main(int argc, char const **argv)
{
    if (argc != 2) printf("Invalid amount of arguments");

    mkfifo(argv[1], 0777); //0666 read and write are enough or 0600
   
    char buffer[BUFFER];
    printf("Master is initializing.\n");

    FILE * file = fopen(argv[1],"r");
	
	while(fgets(buffer,BUFFER,file) != NULL)	fwrite(buffer, sizeof(char), strlen(buffer), stdout);


    fclose(file);
    printf("Master is done\n");

return 0;    
}
