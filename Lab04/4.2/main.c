#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <bits/siginfo.h>

pid_t *processes;
pid_t *waiting;
int counter =0;
int M;
int N;
int continuedProcesses;

void toKillHandler (int sigNumber, siginfo_t *info, void *context);
void childHandler (int sigNumber, siginfo_t *info, void *context);
void appeal (int sigNumber, siginfo_t *info, void *context);
void printer (int sigNumber, siginfo_t *info, void *context);

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        printf("Invalid number of arguments\n");
        exit(1);
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    if (N<M || N<1 || M<0)
    {
        printf("Invalid arguments.\n");
        exit(1);
    }
    processes = calloc((size_t )N, sizeof(int));
    waiting = calloc((size_t )N, sizeof(int));
    pid_t process;
    continuedProcesses = N;

    // catching exceptions

    struct sigaction action;
    action.sa_sigaction = childHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO | SA_NODEFER | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &action, NULL) == -1)
    {
        printf("Cannot catch SIGCHLD.\n");
        exit(1);
    }
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = toKillHandler;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        printf("Cannot catch SIGINT.\n");
        exit(1);
    }

    action.sa_sigaction = appeal;
    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        printf("Cannot catch SIGUSR1 .\n");
        exit(1);
    }

    action.sa_sigaction = printer;

    for (int i = SIGRTMIN; i<= SIGRTMAX; i++) { // catching real time signals
        if (sigaction(i, &action, NULL) == -1)
        {
            printf("Cannot catch SIGRTMIN+%d.\n",i - SIGRTMIN );
            exit(1);
        }
    }


    for (int i=0; i<N; i++)
    {
        usleep(10000);
        process = fork();
        if (process < 0) {
            printf("Cannot fork\n");
            exit(1);
        }

        if (process)
        {
                printf("Starting new procces ID: %d.\n", process);
                processes[i] = process;
        }
        else
        {
            if(execl("./child", "./child", NULL) <0)
                printf("Cannot run ./child\n");
            exit(1);
        }
    }

    while (wait(NULL)); // when there are no child processes, it returns -1 and quits while
 
    return 0;
}

void toKillHandler (int sigNumber, siginfo_t *info, void *context)
{
    int killcounter =0;
    printf("It has gotten signal SIGINT. Killing all processes.\n");
    for (int i=0;i<N; i++)
    {
        if (processes[i] > 0)
        {
            if(kill(processes[i], SIGKILL) == -1)
                killcounter++;
            waitpid(processes[i], NULL, 0);
        }
    }
    printf("All process has been killed.\n");
    exit(0);
}

void childHandler (int sigNumber, siginfo_t *info, void *context)
{
        printf("Process pid %d came back with: %d.\n", info->si_pid, info->si_status);
    if (!continuedProcesses)
        {
            printf("All process are done.\n");
            free(waiting);
            free(processes);
            exit(0);
    }
}



void appeal (int sigNumber, siginfo_t *info, void *context) {
    printf("Got SIGUSR1 from process pid: %d.\n", info->si_pid);
    if (counter < M) {
        waiting[counter++] = info->si_pid;
        if (counter >= M) {
            printf("Too many requests.\n");
            for (int i = 0; i < counter; ++i) {
                if (waiting[i] > 0) {
                    printf("Allowing to send real time signal.\n");
                    kill(waiting[i], SIGUSR1);
                    waitpid(waiting[i], NULL, 0);
                    continuedProcesses--;
                }
            }
        }
    } else {
        kill(info->si_pid, SIGUSR1);
        waitpid(info->si_pid, NULL, 0);
        continuedProcesses--;
    }

}

void printer (int sigNumber, siginfo_t *info, void *context)
{
    printf("Got SIGRTMIN+%d from process pid: %d\n", sigNumber-SIGRTMIN, info->si_pid);
}


