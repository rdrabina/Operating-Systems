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

msg_t msg_rcv();
void barber(int seats);
void err(const char* msg);
void init_resources(void);
void print_msg(char* const msg, int id);
void __exit(void);
void sigexit(int sig) {exit(EXIT_FAILURE);}


int main(int argc, char const *argv[])
{
    if (argc < 2) err("Too few arguments!");
    int seats = atoi(argv[1]);
    if (seats <= 0) err("Invalid argumnt!");

    atexit(__exit);
    signal(SIGTERM, sigexit);
    signal(SIGINT, sigexit);
    signal(SIGSEGV, sigexit);
    init_resources();
    bs->seats_free = seats;

    printf("Barber opens a local\n");

    while (1) barber(seats);


    exit(EXIT_FAILURE);
}

msg_t msg_rcv()
{
    msg_t msg = bs->fifo[bs->rear_q];
    if (bs->rear_q == MAX_CLIENTS-1) bs->rear_q = 0;
    else bs->rear_q++;
    return msg;
}

void barber(int seats)
{
    msg_t msg;
    sem_wait(shop_state);
    if (bs->seats_free == seats)
    {
        print_msg("Barber falls asleep\t\t\t\t\t\t", -1);
        bs->is_asleep = 1;
        sem_post(shop_state);
        sem_wait(cust_present);
        print_msg("Barber wakes up\t\t\t\t\t\t\t", -1);
    }
    else
    {
        msg = msg_rcv();
        bs->served_customer = msg.id;
        print_msg("Barber invites a new client onto the chair\t\t\t", bs->served_customer);
        sem_post(&(bs->prv[msg.sem]));
        sem_post(shop_state);
    }
    sem_post(barb_ready);
    sem_wait(cust_ready);
    print_msg("Barber starts cutting\t\t\t\t\t\t", bs->served_customer);
    print_msg("Barber ends cutting\t\t\t\t\t\t", bs->served_customer);
    sem_post(cut_done);
    sem_wait(cust_ready);
}


void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}


void init_resources(void)
{
    int ss, i;

    if ((ss = shm_open(BARBER_SHARED, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0) err("Shared segment");
    if (ftruncate(ss, sizeof(barber_state)) < 0) err("Ftruncate");
    if ((bs = (barber_state*) mmap(NULL, sizeof(barber_state), PROT_READ | PROT_WRITE, MAP_SHARED, ss, 0)) < 0) err("MMap");
    close(ss);
    bs->is_asleep = 0;
    bs->top = 0;
    bs->top_q = 0;
    bs->rear_q = 0;
    for(i = 0; i<MAX_CLIENTS; i++) sem_init(&(bs->prv[i]), 1, 0);

    shop_state = sem_open(SEM_SHOP, O_CREAT, 0777, 1);
    cust_present = sem_open(SEM_CPRES, O_CREAT, 0777, 0);
    barb_ready = sem_open(SEM_BREAD, O_CREAT, 0777, 0);
    cust_ready = sem_open(SEM_CREAD, O_CREAT, 0777, 0);
    cut_done = sem_open(SEM_CUT, O_CREAT, 0777, 0);
    seat_occupied = sem_open(SEM_SEAT, O_CREAT, 0777, 1);
    queue_seat = sem_open(QUE_SEAT, O_CREAT, 0777, 1);
}

void print_msg(char* const msg, int id)
{
    char buff[100];
    struct timespec tp;
    sprintf(buff, "%s", msg);
    if (id != -1) sprintf(buff, "%s %i", buff, id);
    clock_gettime(CLOCK_MONOTONIC, &tp);
    sprintf(buff, "%s \t Time: %ld:%ld\n", buff, tp.tv_sec, tp.tv_nsec/1000);
    printf("%s", buff);
    fflush(stdout);
}

void __exit(void)
{
    int i;
    for(i = 0; i<MAX_CLIENTS; i++) sem_destroy(&(bs->prv[i]));
    munmap(bs, sizeof(barber_state));
    shm_unlink(BARBER_SHARED);
    sem_close(shop_state);
    sem_close(cust_present);
    sem_close(barb_ready);
    sem_close(cust_ready);
    sem_close(cut_done);
    sem_close(seat_occupied);
    sem_close(queue_seat);
    sem_unlink(SEM_SHOP);
    sem_unlink(SEM_CPRES);
    sem_unlink(SEM_BREAD);
    sem_unlink(SEM_CREAD);
    sem_unlink(SEM_CUT);
    sem_unlink(SEM_SEAT);
    sem_unlink(QUE_SEAT);
}

