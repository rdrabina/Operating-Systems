#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include "specifications.h"


int msgqueue;
int server;
int client_id = -1;

void set_sigint();
void __exit();
void connect_to_server();
int parse_command(char *line, int len, int* pos);
void sig_handler(int sig);
void err(const char* msg);
void receive_reply(struct msgbuf* msg);

int main(int argc, char const *argv[])
{
    FILE *file;
    int intr = 1;
    if (argc > 1)
    {
        file = fopen(argv[1], "r");
        if (file == NULL) err("Cannot open a file");
        intr = 0;
    }
    else file = stdin;

    set_sigint();
    atexit(__exit);

    char *buf = NULL;
    char line[64];
    size_t n;
    struct msgbuf msg;
    int count, pos, msg_id;

    if ((server = msgget(ftok(getenv("HOME"), 0), 0)) < 0) err("There is no opened server");
    if ((msgqueue = msgget(IPC_PRIVATE, S_IRWXU)) < 0) err("Server->client queue");
    
    connect_to_server(); 
    while (printf("> ") < 0 || (count = getline(&buf, &n, file)) > 1)
    {
        sprintf(line, "%.*s", (buf[count-1] == '\n') ? count-1 : count, buf);
        msg_id = parse_command(line, count, &pos);
        
        if (msg_id == UNDEF_MSG)
        {
            printf("Invalid command %s\n", line);
            continue;
        }
        
        msg.mtype = msg_id;
        sprintf(msg.mtext, "%s", line + pos);
        msg.client_id = client_id;
        msg.client_pid = getpid();
        if (msgsnd(server, &msg, MAX_MSG, 0) < 0)
        {
            if (errno == EINVAL && intr)
            {
                printf("Invalid argument\n");
                continue;
            }
            else err("Client send");
        }
        
        receive_reply(&msg);
    }

    printf("Exiting\n");
    exit(EXIT_SUCCESS);
}

void set_sigint()
{
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < -1) err("Signal");
    if (sigaction(SIGSEGV, &act, NULL) < -1) err("Signal");
}

void __exit(void)
{
    msgctl(msgqueue, IPC_RMID, NULL);
    if (client_id == -1) return;
    struct msgbuf msg;
    msg.mtype = STOP_MSG;
    msg.client_id = client_id;
    msg.client_pid = getpid();
    sprintf(msg.mtext, "%i", client_id);
    msgsnd(server, &msg, MAX_MSG, 0);
    exit(0);
}

void connect_to_server()
{
    struct msgbuf msg;

    msg.mtype = INIT_MSG;
    msg.client_id = -1;
    msg.client_pid = getpid();
    sprintf(msg.mtext, "%i", msgqueue);
    if (msgsnd(server, &msg, MAX_MSG, 0) < 0) err("Client send init");

    if (msgrcv(msgqueue, &msg, MAX_MSG, 0, 0) < 0) err("Client receive init");
    switch(msg.mtype)
    {
        case RPLY_MSG:
            client_id = atoi(msg.mtext);
            if (client_id < 0) err("Client register");
            break;
        case ERR_MSG:
            err(msg.mtext);
            break;
        default:
            break;
    }
}

int parse_command(char *line, int len, int* pos)
{
    int index = 0;
    int ret;
    char cmnd[6];

    while (line[index] != ' ' && index != len-1 && index < 6) index++;
    sprintf(cmnd, "%.*s", index, line);
    if (line[index] == ' ') while(line[index] == ' ') index++;
    *pos = index;
    
    if (strcmp(cmnd, "MIRROR") == 0)
        ret = MIRROR_MSG;
    else if (strcmp(cmnd, "CALC") == 0)
        ret  = CALC_MSG;
    else if (strcmp(cmnd, "TIME") == 0)
        ret = TIME_MSG;
    else if (strcmp(cmnd, "END") == 0)
        ret = END_MSG;
    else ret = UNDEF_MSG;

    return ret;
}

void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nExiting\n");
        exit(EXIT_SUCCESS);
    }
    else if (sig == SIGSEGV) err("Segmentation fault");
}

void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

void receive_reply(struct msgbuf* msg)
{
    if (msg->mtype == END_MSG) return;
    if (msgrcv(msgqueue, msg, MAX_MSG, 0, MSG_NOERROR) < 0) perror("Client receive");
    printf("%s\n", msg->mtext);
}

