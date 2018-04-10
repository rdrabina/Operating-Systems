#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void receiver(int signo){
kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}

int main()
{
 	    signal(SIGUSR1, receiver); //getting request of allowing to work
            sigset_t mask;
            sigfillset(&mask);
            sigdelset(&mask, SIGUSR1); //blocking all of signals except SIGUSR1
            int timeToSleep;
            srand((int)(getppid() * getpid()));
            timeToSleep = (uint)rand()%11;
	    printf("Process %d is waiting %d seconds.\n",getppid(),timeToSleep);
            sleep(timeToSleep);
            kill(getppid(), SIGUSR1); // send request of allowing to work
            sigsuspend(&mask); // waiting for SIGUSR1
            return timeToSleep;
}
