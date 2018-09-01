#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "useful_functions.h"

int inetfd, unixfd;
int epoll;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
client clients[MAX_CLIENTS];
int activity_of_clients[MAX_CLIENTS];
int counter = 1;
pthread_t pingthread, commandthread;
char *unixpath;


void init(char *portstr, char *path);
void register_client(int clientfd);
void delete_client(int clientfd);
void receive_message(int clientfd);
void get_result(int clientfd);
void add_to_epoll(int socketfd);
void *ping_clients(void *args);
void *send_commands(void *args);
void intReact(int signum);
void closeall();


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    init(argv[1], argv[2]);
    pthread_create(&pingthread, NULL, ping_clients, NULL);
    pthread_create(&commandthread, NULL, send_commands, NULL);
    atexit(closeall);
    signal(SIGINT, intReact);

    while (1) {
        struct epoll_event event;
        if (epoll_wait(epoll, &event, 1, -1) == -1){
            printf("Epoll failed\n");
            exit(1);
        }
        if (event.data.fd == unixfd) add_to_epoll(unixfd);
        else if (event.data.fd == inetfd) add_to_epoll(inetfd);
        else receive_message(event.data.fd);
    }
}

void init(char *portstr, char *path) {
    unixpath = (char *)calloc(108, sizeof(char));
    strcpy(unixpath, path);

    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i].fd = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i) activity_of_clients[i] = -1;

    uint16_t port = (uint16_t) strtol(portstr, NULL, 10);
    if (port < 1024 || port > 65535) {
        printf("Port failed\n");
        exit(1);
    }
    if (strlen(path) > 108 || strlen(path) < 1) {
        printf("Path name failed\n");
        exit(1);
    }

    if ((inetfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Creating INET socket failed\n");
        exit(1);
    }
    if ((unixfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        printf("Creating UNIX socket failed\n");
        exit(1);
    }

    struct sockaddr_in inetaddr;
    inetaddr.sin_family = AF_INET;
    inetaddr.sin_port = htons(port);
    inetaddr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_un unixaddr;
    unixaddr.sun_family = AF_UNIX;
    strcpy(unixaddr.sun_path, path);

    if (bind(inetfd, (const struct sockaddr *)&inetaddr, sizeof(inetaddr)) == -1 ||
        bind(unixfd, (const struct sockaddr *)&unixaddr, sizeof(unixaddr)) == -1) {
        printf("Binding failed\n");
        exit(1);
    }

    if (listen(inetfd, 10) == -1 || listen(unixfd, 10) == -1) {
        printf("Listening to sockets failed\n");
        exit(1);
    }

    if ((epoll = epoll_create1(0)) == -1) {
        printf("Creating epoll failed\n");
        closeall();
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;

    event.data.fd = inetfd;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, inetfd, &event) == -1) {
        printf("Adding inet socket to epoll failed\n");
        closeall();
        exit(1);
    }

    event.data.fd = unixfd;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, unixfd, &event) == -1) {
        printf("Adding inet socket to epoll failed\n");
        closeall();
        exit(1);
    }
}

void closeall() {
    shutdown(unixfd, SHUT_RDWR);
    shutdown(inetfd, SHUT_RDWR);

    close(inetfd);
    close(unixfd);
    unlink(unixpath);

    pthread_cancel(pingthread);
    pthread_cancel(commandthread);
}

void register_client(int clientfd) {
    uint16_t length;
    if (recv(clientfd, &length, 2, 0) == -1) {
        printf("Receiving failed\n");
        exit(1);
    }

    char *clientname = (char *)calloc(length + 1, sizeof(char));
    if (recv(clientfd, clientname, length, 0) == -1) {
        printf("Receiving failed\n");
        exit(1);
    }

    pthread_mutex_lock(&clients_mutex);
    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd == -1) {
            index = i;
            break;
        }
        if (strcmp(clients[i].name, clientname) == 0) {
            uint8_t type = NAME_IN_USE;
            send(clientfd, &type, 1, 0);
            epoll_ctl(epoll, EPOLL_CTL_DEL, clientfd, NULL);
            shutdown(clientfd, SHUT_RDWR);
            close(clientfd);
            pthread_mutex_unlock(&clients_mutex);
            return;
        }
    }
    if (index == -1) {
        uint8_t type = TOO_MANY_CLIENTS;
        send(clientfd, &type, 1, 0);
        epoll_ctl(epoll, EPOLL_CTL_DEL, clientfd, NULL);
        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
        pthread_mutex_unlock(&clients_mutex);
        return;
    }

    client client;
    client.fd = clientfd;
    strcpy(client.name, clientname);
    clients[index] = client;
    activity_of_clients[index] = 1;
    pthread_mutex_unlock(&clients_mutex);

    uint8_t type = REGISTERED;
    send(clientfd, &type, 1, 0);
    printf("Client %s joined\n", clientname);
    free(clientname);
}

void delete_client(int clientfd) {
    uint16_t length;
    recv(clientfd, &length, 2, 0);
    char *name = (char *)calloc(length + 1, sizeof(char));
    recv(clientfd, name, length, 0);

    epoll_ctl(epoll, EPOLL_CTL_DEL, clientfd, NULL);
    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);

    int deleted = 0;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (strcmp(clients[i].name, name) == 0) {
            deleted = 1;
	    clients[i].fd = -1;
            strcpy(clients[i].name, "");
            activity_of_clients[i] = -1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (deleted) printf("Client %s is deleted from server\n", name);
    free(name);
}

void get_result(int clientfd) {
    uint16_t length;
    recv(clientfd, &length, 2, 0);
    result result;
    recv(clientfd, &result, length, 0);
    int index;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if(clients[i].fd == clientfd) {
            index = i;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    printf("Nr %d from client %s - result: %d\n", result.ID, clients[index].name, result.result);
}

void receive_message(int clientfd) {
    uint8_t type;
    uint16_t length;
    char *name;
    if (recv(clientfd, &type, 1, 0) == -1) {
        printf("Receiving failed\n");
        exit(1);
    }

    switch(type) {
        case REGISTER:
            register_client(clientfd);
            break;
        case ANSWER_TO_PING:
            recv(clientfd, &length, 2, 0);
            name = (char *)calloc(length + 1, sizeof(char));
            recv(clientfd, name, length, 0);
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if(strcmp(clients[i].name, name) == 0) {
                    activity_of_clients[i] = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            break;
        case DELETE:
            delete_client(clientfd);
            break;
        case RESULT:
            get_result(clientfd);
            break;
        default:
            break;
    }
}

void add_to_epoll(int socketfd) {
    int clientfd = accept(socketfd, NULL, NULL);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = clientfd;

    epoll_ctl(epoll, EPOLL_CTL_ADD, clientfd, &event);
}

void *ping_clients(void *args) {
    while (1) {
        sleep(5);
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (activity_of_clients[i] == 1) {
                uint8_t type = PING;
                send(clients[i].fd, &type, 1, 0);
                activity_of_clients[i] = 0;
            } else if (activity_of_clients[i] == 0) {
		printf("Client %s is deleted because of inactivity\n", clients[i].name);
                epoll_ctl(epoll, EPOLL_CTL_DEL, clients[i].fd, NULL);
                shutdown(clients[i].fd, SHUT_RDWR);
                close(clients[i].fd);

                clients[i].fd = -1;
                strcpy(clients[i].name, "");
		activity_of_clients[i] = -1;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(5);
    }
}

void *send_commands(void *args) {
    srand((unsigned int)time(NULL));
    operation operation;
    while(1) {
        operation.ID = counter;
        counter++;
        struct timespec time;
        time.tv_nsec = 100000000;
        time.tv_sec = 0;
        nanosleep(&time, NULL);
        printf("Type: ");
        char *command = (char *)calloc(200, sizeof(char));
        fgets(command, 200, stdin);
	if (strchr(command, '+') == NULL && strchr(command, '-') == NULL && strchr(command, '/') == NULL && strchr(command, '*') == NULL) {
	    printf("Wrong command\n");
            continue;	
	}
        int tmp = (int) strcspn(command, "+-/*");
        char op = command[tmp];
        int arg1 = (int) strtol(strtok(command, " +-/*"), NULL, 10);
        int arg2 = (int) strtol(strtok(NULL, " +-/*"), NULL, 10);
        operation.arg1 = arg1;
        operation.arg2 = arg2;
        operation.operator = op;

        uint8_t type = ORDER;
        uint16_t length = (uint16_t) sizeof(operation);

        int clientfd = -1;
        while (clientfd == -1) {
            if ((clientfd = clients[rand()%MAX_CLIENTS].fd) != -1) {
                pthread_mutex_lock(&clients_mutex);
            }
        }

        send(clientfd, &type, 1, 0);
        send(clientfd, &length, 2, 0);
        send(clientfd, &operation, length, 0);
        pthread_mutex_unlock(&clients_mutex);
    }
}

void intReact(int signum) {
    printf("\nServer stop.\n");
    exit(0);
}
