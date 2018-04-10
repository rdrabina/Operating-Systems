#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <zconf.h>


int childReceived = 0;
int parentReceived = 0;


void doSigusr1Type1(int signum);
void doSigusr1Type2(int signum);
void doSigrtmin(int signum);
void doSigusr2(int signum);
void doParentSigusr(int signum);
void setSignal(sigset_t* set, int type, int child);
void type1or2(int signalNumber, int type);
void type3(int signalNumber);
void intReact(int signum);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Invalid number of arguments\n");
        exit(1);
    }


    int type = atoi(argv[2]);
    if (type < 1 || type > 3) {
        printf("Invalid number! It should be 1, 2 or 3\n");
        exit(1);
    }
	int signalNumber = atoi(argv[1]);

    sigset_t set;
    setSignal(&set, type, 0);

    struct sigaction action;
    action.sa_handler = intReact;
    sigfillset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    if (type == 1 || type == 2) type1or2(signalNumber, type);
    else type3(signalNumber);

   
    return 0;
}

void doSigusr1Type1(int signum) {
    kill(getppid(), SIGUSR1);
    childReceived++;
    printf("Signals received from parent: %d\n", childReceived);
}

void doSigusr1Type2(int signum) {
    childReceived++;
    printf("Signals received from parent: %d\n", childReceived);
    kill(getppid(), SIGUSR1);
}

void doSigrtmin(int signum) {
    kill(getppid(), SIGRTMIN);
    childReceived++;
    printf("Signals received from parent: %d\n", childReceived);
}

void doSigusr2(int signum) {
    exit(0);
}

void doParentSigusr(int signum) {
    parentReceived++;
    printf("Signals received from child: %d\n", parentReceived);
}


void setSignal(sigset_t* set, int type, int child) {
    sigfillset(set);
   
    if (type == 1 || type == 2) {
		sigdelset(set, SIGUSR1);
        sigdelset(set, SIGUSR2);
    } else {
        sigdelset(set, SIGRTMIN);
        sigdelset(set, SIGRTMAX);
        sigdelset(set, SIGUSR2);
    }
    sigprocmask(SIG_SETMASK, set, NULL);
	if (child == 0) sigdelset(set, SIGINT);
}

void type1or2(int signalNumber, int type) {
    pid_t child = fork();
    if (child == 0) {
        sigset_t set;
        setSignal(&set, 1, 1);
        if (type == 1) signal(SIGUSR1, doSigusr1Type1);
        else signal(SIGUSR1, doSigusr1Type2);

        signal(SIGUSR2, doSigusr2);

        while(1) {};
    } else {
        sleep(1);
        signal(SIGUSR1, doParentSigusr);
        for (int i = 1; i <= signalNumber; ++i) {
            kill(child, SIGUSR1);
            printf("Signals sent to child: %d\n", i);
            if(type == 2) pause();
        }
        kill(child, SIGUSR2);
        wait(NULL);
    }
}

void type3(int signalNumber) {
    pid_t child = fork();
    if (!child) {
        sigset_t set;
        setSignal(&set, 3, 1);

        struct sigaction action;
        action.sa_handler = doSigrtmin;
        sigfillset(&action.sa_mask);
        sigdelset(&action.sa_mask, SIGINT);
        action.sa_flags = 0;

        sigaction(SIGRTMIN, &action, NULL);
        signal(SIGRTMAX, doSigusr2);
        signal(SIGUSR2, doSigusr2);

        while(1) {};

    } else {
        sleep(1);
        signal(SIGRTMIN,doParentSigusr);

        for (int i = 1; i <= signalNumber; ++i) {
            kill(child, SIGRTMIN);
            printf("Signals sent to child: %d\n", i);
        }

        kill(child, SIGRTMAX);
        wait(NULL);
    }
}

void intReact(int signum) {
    kill(0, SIGUSR2);
    exit(1);
}


