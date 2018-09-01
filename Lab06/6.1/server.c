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

void set_sigint();
void __exit(void){msgctl(msgqueue, IPC_RMID, NULL);}
void register_client(int *clients, struct msgbuf* msg);
void remove_client(int *clients, struct msgbuf* msg);
void mirror(int client_queue_id, char* msg_text);
void calc(int clqid, char* msg, int len);
void time(int client_queue_id);
void err(const char* msg);
void snd_err(const char* msgtext, int msgqid);
void sig_handler(int sig);

int main(int argc, char const *argv[])
{
    const char *requests[4] = {"MIRROR", "CALC", "TIME", "END"};
    int i;
    struct msgbuf msg;
    int clients[MAX_CLIENTS];
    for (i = 0; i < MAX_CLIENTS; i++) clients[i] = -1;

    set_sigint();
    atexit(__exit);

    if ((msgqueue = msgget(ftok(getenv("HOME"), 0), IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO)) < 0) err("Server queue open");
    
    while(1)
    {
        if (msgrcv(msgqueue, &msg, MAX_MSG, 0, MSG_NOERROR) < 0) err("Server receive");
        if (msg.mtype < 5) printf("Received %s from #%i\n", requests[msg.mtype-1], msg.client_id);
        switch (msg.mtype)
        {
            case INIT_MSG:
                register_client(clients, &msg);
                break;
            case STOP_MSG:
                remove_client(clients, &msg);
                break;
            case MIRROR_MSG:
                mirror(clients[msg.client_id], msg.mtext);
                break;
            case CALC_MSG:
                calc(clients[msg.client_id], msg.mtext, strlen(msg.mtext));
                break;
            case TIME_MSG:
                time(clients[msg.client_id]);
                break;
            case END_MSG:
                printf("Closing server\n");
    		exit(EXIT_SUCCESS);
           default:
                printf("Invalid request: %s\n", msg.mtext);
                break;
        }
    }
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


void register_client(int *clients, struct msgbuf* msg)
{
    int i, tmp;
    tmp = atoi(msg->mtext);
    for (i = 0; i < MAX_CLIENTS && clients[i] != -1; i++) {;}
    if (i < MAX_CLIENTS)
    {
        clients[i] = tmp;
        msg->mtype = RPLY_MSG;
        sprintf(msg->mtext, "%i", i);
        if (msgsnd(clients[i], msg, MAX_MSG, 0) < 0)
        {
            perror("Init msg");
            clients[i] = -1;
            return;
        }
        printf("Client #%i joined to server: %i\n", i, msg->client_pid);
    }
    else
    {
        msg->mtype = ERR_MSG;
        sprintf(msg->mtext, "Queue is full");
        if (msgsnd(tmp, msg, MAX_MSG, 0)) perror("Init msg");
    }
}

void remove_client(int *clients, struct msgbuf* msg)
{
    if (msg->client_id < 0 || msg->client_id > MAX_CLIENTS) return;
    printf("Client #%i left: %i\n", msg->client_id, msg->client_pid);
    clients[msg->client_id] = -1;
}

void mirror(int client_queue_id, char* msg_text)
{
    char *tmp1 = msg_text;
    char *tmp2 = msg_text + strlen(msg_text) - 1;

    while (tmp1 < tmp2) {
        char tmp = *tmp1;
        *tmp1++ = *tmp2;
        *tmp2-- = tmp;
    }
    struct msgbuf msg;
    msg.mtype = RPLY_MSG;
    sprintf(msg.mtext, "%s", msg_text);
    if (msgsnd(client_queue_id, &msg, MAX_MSG, 0) < 0) perror("Server send");
}

void calc(int clqid, char* msg, int len)
{
    char opr, tmp[10];
    int index = 0, n1, n2, res;
    while (msg[index] >= '0' && msg[index] <= '9' && index < len)index++; 
    if (index == 0) {snd_err("Invalid message", clqid); return;}
    sprintf(tmp, "%.*s",(index > 10) ? index : 10, msg);
    n1 = atoi(tmp);

    while (msg[index] == ' ' && index < len) index++;
    if (index == len) {snd_err("Invalid message", clqid); return;}
    opr = msg[index++];

    while (msg[index] == ' ' && index < len) index++;
    if (index == len || msg[index] < '0' || msg[index] > '9') {snd_err("Invalid message", clqid); return;}
    sprintf(tmp, "%.*s", (len-index > 10) ? len-index : 10, msg + index);
    n2 = atoi(tmp);

    switch (opr)
    {
        case '+': res = n1 + n2; break;
        case '-': res = n1 - n2; break;
        case '/': if (n2) res = n1 / n2; else {snd_err("Arithmetic error", clqid); return;} break;
        case '*': res = n1 * n2; break;
        default: snd_err("Invalid message", clqid); return;
    }
    struct msgbuf msgs;
    msgs.mtype = RPLY_MSG;
    sprintf(msgs.mtext, "%i", res);
    if (msgsnd(clqid, &msgs, MAX_MSG, 0) < 0) perror("Server send");
}

void time(int client_queue_id)
{
    struct msgbuf msg;
    msg.mtype = RPLY_MSG;

    FILE *date = popen("date", "r");
    if (date == NULL || date < 0) perror("date");
    fgets(msg.mtext, MAX_MSG_TXT, date);
    pclose(date);
    sprintf(msg.mtext, "%.*s", (int)strlen(msg.mtext)-1, msg.mtext);
    if (msgsnd(client_queue_id, &msg, MAX_MSG, 0) < 0) perror("Server send");
}

void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

void snd_err(const char* msgtext, int msgqid)
{
    struct msgbuf msg;
    msg.mtype = ERR_MSG;
    sprintf(msg.mtext, "%s", msgtext);
    msgsnd(msgqid, &msg, MAX_MSG, 0);
}

void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nClosing server\n");
        exit(EXIT_SUCCESS);
    }
    else if (sig == SIGSEGV) err("Segmentation fault");
}
