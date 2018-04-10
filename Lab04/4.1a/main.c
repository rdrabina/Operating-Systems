#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


void sigInt();
void signalHandling();

int shouldWait = 0;


int main(int argc, char** argv) {

    time_t ordinaryTime;
    char buffer[30];

    struct sigaction action;
    action.sa_handler = signalHandling;

    sigfillset(&action.sa_mask);
    sigdelset(&action.sa_mask, SIGINT);

    action.sa_flags = 0;

    while(1){
        sigaction(SIGTSTP, &action,NULL);
        signal(SIGINT,sigInt);


        if(shouldWait)
            continue;

        ordinaryTime = time(NULL);
	printf("Current time: ");
        strftime(buffer, sizeof(buffer),"%H:%M:%S",localtime(&ordinaryTime));
        printf("%s\n",buffer);
        sleep(1);
    }
    return 0;
}

void signalHandling()//int sigNum) {
	{if(shouldWait == 0)
	{		
		printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
	}

	if(shouldWait) shouldWait = 0;
	else shouldWait = 1;

return;
}

void sigInt()//int sigNum) {
   { printf("\nOdebrano sygnał SIGINT\n");
    exit(0);
}
