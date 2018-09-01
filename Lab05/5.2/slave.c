#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <zconf.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>


#define BUFFER 1000

int main(int argc, char ** argv){
    if(argc!=3){
        printf("Invalid amount of arguments");
        exit(1);
    }

    	char buffer[BUFFER];
	char printed_date[BUFFER];

	int handle = open(argv[1],O_WRONLY);

	printf("PID: %d\n", getpid());
	srand(time(NULL));

    for(int i=0;i<atoi(argv[2]);i++){
        	FILE * date = popen("date", "r");
		fgets(printed_date, BUFFER, date);
		sprintf(buffer,"PID: %d : %s", getpid(), printed_date);
		write(handle, buffer, strlen(buffer));
		sleep(rand()%4 + 2);
		pclose(date);
    }
    close(handle);

return 0;
}
