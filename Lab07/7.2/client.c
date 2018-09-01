#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sched.h>
#include "posix.h"

barber_state* bs;
sem_t* shop_state;
sem_t* cust_present;
sem_t* barb_ready;
sem_t* cust_ready;
sem_t* cut_done;
sem_t* seat_occupied;
sem_t* queue_seat;

void msg_snd(msg_t msg);
void client(int cuts);
void print_msg(char* const msg);
int get_sem(void);
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

void msg_snd(msg_t msg)
{
    bs->fifo[bs->top_q] = msg;
    if (bs->top_q == MAX_CLIENTS-1) bs->top_q = 0;
    else bs->top_q++;
}

void client(int cuts)
{
    printf("Client %i appears in the local\n", getpid());
    int prvsem = get_sem(), count = 0;
    msg_t msg;
    msg.id = getpid();
    msg.sem = prvsem;
    while (cuts > 0)
    {
        sem_wait(shop_state);
        if (bs->is_asleep)
        {
        	count = 0;
            print_msg("Client wakes the barber\t\t\t");
            bs->is_asleep = 0;
            bs->served_customer = getpid();
            sem_wait(queue_seat);
            sem_post(cust_present);
            sem_post(shop_state);
        }
        else
        {
            if (bs->seats_free)
            {
                sem_wait(queue_seat);
            	count = 0;
                print_msg("Client waits in the queue\t\t");
                msg_snd(msg);
                bs->seats_free -= 1;
                sem_post(queue_seat);
                sem_post(shop_state);
                sem_wait(&(bs->prv[prvsem]));
                sem_wait(shop_state);
                sem_wait(queue_seat);
                bs->seats_free += 1;
                sem_post(shop_state);
            }
            else
            {
                print_msg("Client leaves without a new haircut\t");
                sem_post(shop_state);
                if (count > 50) sched_yield();
                else count++;
                continue;
            }
        }
        sem_wait(seat_occupied);
        sem_wait(barb_ready);
        print_msg("Client takes the seat\t\t\t");
        sem_post(queue_seat);
        sem_post(cust_ready);
        sem_wait(cut_done);
        print_msg("Client leaves with a new haircut\t");
        sem_post(seat_occupied);
        sem_post(cust_ready);
        cuts--;
    }

    printf("Client %i exit\n", getpid());
    _exit(EXIT_SUCCESS);
}

void print_msg(char* const msg)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("%s %i \t Time: %ld:%ld\n", msg, getpid(), tp.tv_sec, tp.tv_nsec/1000);
    fflush(stdout);
}

int get_sem(void)
{
    if (bs->top == MAX_CLIENTS-1) bs->top = 0;
    else bs->top++;
    return bs->top;
}

void init_resources(void)
{
    int ss;
    
    if ((ss = shm_open(BARBER_SHARED, O_RDWR, 0)) < 0) err("Shared segment");
    if ((bs = (barber_state*) mmap(NULL, sizeof(barber_state), \
        PROT_READ | PROT_WRITE, MAP_SHARED, ss, 0)) < 0) err("MMap");
    close(ss);

    shop_state = sem_open(SEM_SHOP, 0);
    cust_present = sem_open(SEM_CPRES, 0);
    barb_ready = sem_open(SEM_BREAD, 0);
    cust_ready = sem_open(SEM_CREAD, 0);
    cut_done = sem_open(SEM_CUT, 0);
    seat_occupied = sem_open(SEM_SEAT, 0);
    queue_seat = sem_open(QUE_SEAT, 0);
}

void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

void __exit(void)
{
    munmap(bs, sizeof(barber_state));
    sem_close(shop_state);
    sem_close(cust_present);
    sem_close(barb_ready);
    sem_close(cust_ready);
    sem_close(cut_done);
    sem_close(seat_occupied);
    sem_close(queue_seat);
}

