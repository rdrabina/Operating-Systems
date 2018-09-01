#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "systemv.h"

int fifo = 0, semid, semprv;
barber_state* bs;

void client(int cuts);
int get_sem(void);
void print_msg(char* const msg);
void init_resources(void);
void err(const char* msg);
void __exit(void);
void sigexit(int sig) {exit(EXIT_FAILURE);}


int main(int argc, char const *argv[])
{
    if (argc < 3) err("Too few arguments!");
    int clients = atoi(argv[1]);
    int cuts = atoi(argv[2]);
    if (clients <= 0 || cuts  <= 0) err("Invalid argumnts!");

    atexit(__exit);
    signal(SIGTERM, sigexit);
    signal(SIGSEGV, sigexit);
    signal(SIGINT, sigexit);
    init_resources();

    int i;
    pid_t tmp;

    for (i = 0; i < clients; i++)
    {
        tmp = fork();
        if (tmp < 0) err("Fork");
        if (tmp == 0) client(cuts);
    }

    exit(EXIT_SUCCESS);
}

void client(int cuts)
{
    printf("Client %i appears in the local\n", getpid());
    int prvsem = get_sem();
    msg_t msg;
    msg.id = getpid();
    msg.sem = prvsem;
    struct sembuf sems[2];
    sems[0].sem_flg = sems[1].sem_flg = 0;
    while (cuts > 0)
    {
        SEMWAIT(semid, SEM_SHOP)
        if (bs->is_asleep)
        {
            print_msg("Client wakes the barber\t\t\t");
            bs->is_asleep = 0;
            bs->served_customer = getpid();
            SEMWAIT(semid, QUE_SEAT)
            SEMPOST2(semid, SEM_SHOP, SEM_CPRES)
        }
        else
        {
            if (bs->seats_free)
            {
                SEMWAIT(semid, QUE_SEAT)
                print_msg("Client waits in the queue\t\t");
                write(fifo, &msg, sizeof(msg_t));
                bs->seats_free -= 1;
                SEMPOST2(semid, SEM_SHOP, QUE_SEAT)
                SEMWAIT(semprv, prvsem)
                SEMWAIT2(semid, SEM_SHOP, QUE_SEAT)
                bs->seats_free += 1;
                SEMPOST(semid, SEM_SHOP)
            }
            else
            {
                print_msg("Client leaves without a new haircut\t");
                SEMPOST(semid, SEM_SHOP)
                continue;
            }
        }
        SEMWAIT2(semid, SEM_BREAD, SEM_SEAT)
        print_msg("Client takes the seat\t\t\t");
        SEMPOST2(semid, QUE_SEAT, SEM_CREAD)
        SEMWAIT(semid, SEM_CUT)
        print_msg("Client leaves with a new haircut\t");
        SEMPOST2(semid, SEM_CREAD, SEM_SEAT)
        cuts--;
    }

    printf("Client %i exit\n", getpid());
    _exit(EXIT_SUCCESS);
}

int get_sem(void)
{
    if (bs->top == MAX_CLIENTS-1) bs->top = 0;
    else bs->top++;
    return bs->top;
}

void print_msg(char* const msg)
{
    char buff[100];
    struct timespec tp;
    sprintf(buff, "%s %i", msg, getpid());
    clock_gettime(CLOCK_MONOTONIC, &tp);
    sprintf(buff, "%s \t Time: %ld:%ld\n", buff, tp.tv_sec, tp.tv_nsec/1000);
    printf("%s", buff);
    fflush(stdout);
}

void init_resources(void)
{    
    int ss;
    if ((fifo = open(FIFO_PATH, O_WRONLY)) < 0) err("Client pipe");
    if ((ss = shmget(ftok(SPATH, 0), 0, 0)) < 0) err("Shmget");
    if ((bs = (barber_state*)shmat(ss, NULL, 0)) < 0) err("Shmat");
    if ((semid = semget(ftok(SPATH, 1), 0, 0)) < 0) err("Semget");
    if ((semprv = semget(ftok(SPATH, 2), 0, 0)) < 0) err("Semget");
}

void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

void __exit(void)
{
    if (fifo) close(fifo);
    shmdt(bs);
}
