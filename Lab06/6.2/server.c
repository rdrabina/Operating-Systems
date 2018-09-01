#include <mqueue.h>
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


mqd_t msgqueue;
mqd_t clients[MAX_CLIENTS];

void set_sigint();
void __exit(void);
void register_client(char* msg);
void remove_client(char* msg);
void mirror(mqd_t queue_id, char* msg_text);
void calc(mqd_t clqid, char* msg, int len);
void time(mqd_t queue_id);
void err(const char* msg);
void snd_err(const char* msgtext, mqd_t msgqid);
void sig_handler(int sig);

int main(int argc, char const *argv[])
{
    const char *requests[4] = {"MIRROR", "CALC", "TIME", "END"};
    int i;
    char type, msg[MAX_MSG];
    for (i = 0; i < MAX_CLIENTS; i++) clients[i] = (mqd_t)-1;

    set_sigint();
    atexit(__exit);

    if ((msgqueue = mq_open(SERVER_NAME, O_CREAT | O_EXCL | O_RDONLY, S_IRUSR | S_IWUSR, NULL)) < 0) err("Server queue open");

    while(1)
    {
        if (mq_receive(msgqueue, msg, MAX_MSG, NULL) < 0) err("Server receive");
        type = msg[0];
        if (type < 5) printf("Received %s from " \
            "#%i\n", requests[type-1], (int)msg[1]);
        switch (type)
        {
            case INIT_MSG:
                register_client(msg);
                break;
            case STOP_MSG:
                remove_client(msg);
                break;
            case MIRROR_MSG:
                mirror((int)clients[(int)msg[1]], msg + 2);
                break;
            case CALC_MSG:
                calc((int)clients[(int)msg[1]], msg + 2, strlen(msg + 2));
                break;
            case TIME_MSG:
                time((int)clients[(int)msg[1]]);
                break;
            case END_MSG:
                 printf("Closing server\n");
    		 exit(EXIT_SUCCESS);
            default:
                printf("Invalid request: %s\n", msg + 2);
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
}

void __exit(void)
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != -1) if (mq_close(clients[i]) < 0) perror("Client remove");
    mq_close(msgqueue);
    mq_unlink(SERVER_NAME);
}

void register_client(char* msg)
{
    int i;
    char tmp_name[MAX_MSG-2];
    mqd_t tmp;
    sprintf(tmp_name, "%s", msg+2);
    if ((tmp = mq_open(tmp_name, O_WRONLY)) < 0) err("Server->client queue open");
    for (i = 0; i < MAX_CLIENTS && clients[i] != -1; i++) {;}
    if (i < MAX_CLIENTS)
    {
        clients[i] = tmp;
        msg[0] = RPLY_MSG;
        sprintf(msg+2, "%i", i);
        if (mq_send(clients[i], msg, MAX_MSG, 1) < 0)
        {
            perror("Init msg");
            clients[i] = (mqd_t) -1;
            return;
        }
        printf("Client #%i joined to server:\n", i);
    }
    else
    {
        msg[0] = ERR_MSG;
        sprintf(msg+2, "Queue is full");
        if (mq_send(tmp, msg, MAX_MSG, 1)) perror("Init msg");
    }
}

void remove_client(char* msg)
{
    if (msg[1] < 0 || msg[1] > MAX_CLIENTS) return;
    int client_id = (int)msg[1];
    if (mq_close(clients[client_id]) < 0) perror("Remove client");
    printf("Client #%i left\n", client_id);
    clients[(int)msg[1]] = -1;
}

void mirror(mqd_t queue_id, char* msg_text)
{
    char *tmp1 = msg_text;
    char *tmp2 = msg_text + strlen(msg_text) - 1;

    while (tmp1 < tmp2) {
        char tmp = *tmp1;
        *tmp1++ = *tmp2;
        *tmp2-- = tmp;
    }
    char msg[MAX_MSG];
    msg[0] = RPLY_MSG;
    sprintf(msg+2, "%s", msg_text);
    if (mq_send(queue_id, msg, MAX_MSG, 1) < 0) perror("Server send");
}

void calc(mqd_t clqid, char* msg, int len)
{
    char opr, tmp[10];
    int index = 0, n1, n2, res;
    while (msg[index] >= '0' && msg[index] <= '9' && index < len) index++;
    if (index == 0) {snd_err("Malformed message", clqid); return;}
    sprintf(tmp, "%.*s",(index > 10) ? index : 10, msg);
    n1 = atoi(tmp);

    while (msg[index] == ' ' && index < len) index++;
    if (index == len) {snd_err("Malformed message", clqid); return;}
    opr = msg[index++];

    while (msg[index] == ' ' && index < len) index++;
    if (index == len || msg[index] < '0' || msg[index] > '9') {snd_err("Malformed message", clqid); return;}
    sprintf(tmp, "%.*s", (len-index > 10) ? len-index : 10, msg + index);
    n2 = atoi(tmp);

    switch (opr)
    {
        case '+': res = n1 + n2; break;
        case '-': res = n1 - n2; break;
        case '/': if (n2) res = n1 / n2; else {snd_err("Arithmetic error", clqid); return;} break;
        case '*': res = n1 * n2; break;
        default: snd_err("Malformed message", clqid); return;
    }
    char msgs[MAX_MSG];
    msgs[0] = RPLY_MSG;
    sprintf(msgs+2, "%i", res);
    if (mq_send(clqid, msgs, MAX_MSG, 1) < 0) perror("Server send");
}

void time(int client_queue_id)
{
    char msg[MAX_MSG];
    msg[0] = RPLY_MSG;

    FILE *date = popen("date", "r");
    if (date == NULL || date < 0) perror("date");
    fgets(msg+2, MAX_MSG-2, date);
    pclose(date);

    sprintf(msg+2, "%.*s", (int)strlen(msg+2)-1, msg+2);
    if (mq_send(client_queue_id, msg, MAX_MSG, 1) < 0) perror("Server send");
}

void err(const char* msg)
{
    if (errno) perror(msg);
    else printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

void snd_err(const char* msgtext, mqd_t msgqid)
{
    char msg[MAX_MSG];
    msg[0] = ERR_MSG;
    sprintf(msg+2, "%s", msgtext);
    if(mq_send(msgqid, msg, MAX_MSG, 1) < 0) perror("Error send");
}

void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nClosing server\n");
        exit(EXIT_SUCCESS);
    }
}

