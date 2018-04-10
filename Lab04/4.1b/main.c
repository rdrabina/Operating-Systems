#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


void sigInt();
void signalHandling();

int shouldWait = 0;
int noChild = 1;
pid_t pid = 0;


int main(int argc, char** argv) {

    struct sigaction action;
    action.sa_handler = signalHandling;

	sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

	pid = fork();
	if(!pid)
	{
		execl("./time.sh", "./time.sh", (char *) NULL);
		exit(0);
	}

    while(1)
	{
        sigaction(SIGTSTP, &action,NULL);
        signal(SIGINT,sigInt);


		if(!shouldWait)
		{
			if(noChild)
			{
				shouldWait = 0;
				noChild = 0;

				pid = fork();
				if(!pid)
				{
					execl("./time.sh", "./time.sh", (char *) NULL);
					exit(0);
				}
			}
		}
	
		else
		{
			if(!noChild)
			{
				noChild = 1;			
				kill(pid, SIGKILL);
			}
		}
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
   { 
	printf("\nOdebrano sygnał SIGINT\n");
	if(!noChild) kill(pid, SIGKILL);

	exit(0);
}
