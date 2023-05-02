//
// Created by muragara on 4/15/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/wait.h>

#include "inputHandler.h"
#include "common.h"
#include "Semaphore.h"
#include "cmdEnum.h"
#include "subStore.h"

#define TRUE 1
#define PORT 5678
#define MAX_CLIENTS 128
#define SEGSIZE sizeof(Message) * ARRSIZE

semaphore mutex;
semaphore block;
semaphore subStoreMutex;

int *blocker = 0;

sig_atomic_t endProgram = 0;

void signal_handler(int signum) {
    endProgram = 1;
}

void sigchld_handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handleError(int bool, char *msg) {
    if (bool) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void notify(SubscriberStore *subscriberStore, SubMessage subMessage){
    for (int i = 0; i < MAXKEYS; i++) {
        if(strcmp(subscriberStore[i].key, "") == 0)
            return;

        if (strcmp(subscriberStore[i].key, subMessage.key) == 0) {
            PubMessage pubMessage = {
                    .mtype = 1,
                    .mtext = "",
                    .keyIndex = i,
            };

            snprintf(pubMessage.mtext, BUFSIZE, "%s:%s:%s\n", getCmdString(subMessage.cmd), subMessage.key, subMessage.value);

            for (int j = 0; j < MAXSUBS; j++) {
                if (subscriberStore[i].subs[j].pid == 0)
                    break;
                msgsnd(subscriberStore[i].subs[j].clientQueue, &pubMessage, sizeof(PubMessage), 0);
            }
        }
    }
}

//handle queue
void handleQueue(int qid, SubscriberStore *subscriberStore) {
    SubMessage subMessage = {
            .mtype = 1,
            .value = "",
            .key = "",
            .cmd = -1
    };

    while (!endProgram) {
        if (msgrcv(qid, &subMessage, sizeof(SubMessage), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_SUCCESS);
        }

        printf("Received: %s\n", subMessage.key);

        if(strcmp(subMessage.key, "") != 0){
            down(subStoreMutex, 0);
            notify(subscriberStore, subMessage);
            up(subStoreMutex, 0);
        }

    }
}

void handleClient(int cfd, Message *messages, SubscriberStore *subscriberStore, int subQueue) {
    //message queue for publisher
    int clientQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    handleError(clientQueue < 0, "msgget failed");

    //fork
    pid_t pid = fork();
    handleError(pid < 0, "Error forking");

    // child process
    if (pid == 0) {
        while(!endProgram){
            PubMessage pubMessage = {
                    .mtype = 0,
                    .mtext = "",
                    .keyIndex = 0,
            };

            if (msgrcv(clientQueue, &pubMessage, sizeof(PubMessage), 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            printf("Client Process %i recieved %s\n", getppid(), pubMessage.mtext);

            down(subStoreMutex, 0);
            int t = write(cfd, pubMessage.mtext, BUFSIZE);
            if (t < BUFSIZE) {
                printf("Warning: Not all bytes written to socket. Expected: %d, Actual: %ld\n", BUFSIZE, t);
            }
            up(subStoreMutex, 0);
        }
    }

    char in[BUFSIZE]; // Client Data
    int bytes_read; // number read, -1 for errors or 0 for EOF
    printf("Client %d connected\n", cfd);
    memset(in, 0, BUFSIZE);
    bytes_read = read(cfd, in, BUFSIZE);

    int action = -1;

    while (bytes_read > 0) {
        // wait until the client is blockerhandleQueue or no one is blocker
        while (*blocker != 0 && *blocker != getpid()) {
            sleep(1);
        }

        down(mutex, 0);
        action = handleInput(messages, subscriberStore, in, subQueue, clientQueue);
        up(mutex, 0);
        printf("action: %i\n", action);

        write(cfd, in, sizeof(in));

        switch (action) {
            case QUIT:
                printf("Client %d quit\n", getpid());
                printf("Killing child process %d\n", pid);
                kill(pid, SIGKILL); // kill the child process
                msgctl(clientQueue, IPC_RMID, NULL);
                return;
            case BEG:
                down(block, 0);
                *blocker = getpid();
                up(block, 0);
                break;
            case END:
                down(block, 0);
                *blocker = 0;
                up(block, 0);
                break;
            default:
                break;
        }

        memset(in, 0, BUFSIZE);
        bytes_read = read(cfd, in, BUFSIZE);
    }
    close(cfd);
    msgctl(clientQueue, IPC_RMID, NULL);
}


int createSharedArray(Message **sharedArray) {
    int sharedArrayID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    handleError(sharedArrayID < 0, "shmget failed");
    *sharedArray = (Message *) shmat(sharedArrayID, 0, 0);
    handleError(*sharedArray == (Message *) -1, "shmat failed");
    return sharedArrayID;
}

int createSharedBlocker(int **blocker) {
    int blockerID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    handleError(blockerID < 0, "shmget failed");
    *blocker = (int *) shmat(blockerID, 0, 0);
    handleError(*blocker == (int *) -1, "shmat failed");
    return blockerID;
}

void initArray(Message *sharedArray) {
    for (int i = 0; i < ARRSIZE; ++i) {
        Message message = {
                .key = "",
                .value = ""
        };
        sharedArray[i] = message;
    }
}

int createSharedSubscriberStore(SubscriberStore **subscriberStore) {
    int subscriberStoreID = shmget(IPC_PRIVATE, sizeof(SubscriberStore) * MAXSUBS, IPC_CREAT | 0600);
    handleError(subscriberStoreID < 0, "shmget failed");
    *subscriberStore = (SubscriberStore *) shmat(subscriberStoreID, 0, 0);
    handleError(*subscriberStore == (SubscriberStore *) -1, "shmat failed");
    return subscriberStoreID;
}

void initSubscriberStore(SubscriberStore *subscriberStore) {
    for (int i = 0; i < ARRSIZE; i++) {
        SubscriberStore tempSubStore = {
                .key = "",
                .subs = {}
        };
        subscriberStore[i] = tempSubStore;
    }
}

void deleteSharedMemory(int memId){
    int t = shmctl(memId, IPC_RMID, 0);
    handleError(t < 0, "shmctl failed");
}

void deleteSemaphores(int semId){
    int t = semctl(semId, 0, IPC_RMID, 0);
    handleError(t < 0, "semctl failed");
}

void Server() {
     signal(SIGTERM, signal_handler);
     signal(SIGCHLD, sigchld_handler);


    //shared array
    Message *sharedArray;
    int sharedArrayID = createSharedArray(&sharedArray);
    initArray(sharedArray);

    //shared subscriber array
    SubscriberStore *subscriberStore;
    int subscriberStoreID = createSharedSubscriberStore(&subscriberStore);
    initSubscriberStore(subscriberStore);

    //mutex
    mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    handleError(mutex < 0, "semget failed");
    semctl(mutex, 0, SETVAL, 1);

    //block
    block = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    handleError(block < 0, "semget failed");
    semctl(block, 0, SETVAL, 1);
    semctl(block, 1, SETVAL, 0);

    //blocker
    blocker = (int *) malloc(sizeof(int));
    int blockerID = createSharedBlocker(&blocker);
    *blocker = 0;

    //substore mutex
    subStoreMutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    handleError(subStoreMutex < 0, "semget failed");
    semctl(subStoreMutex, 0, SETVAL, 1);


    //sockets
    int rfd, cfd;
    struct sockaddr_in server, client;
    unsigned int len;

    //create socket
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    handleError(rfd < 0, "socket failed");

    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    int t = setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));
    handleError(t < 0, "setsockopt failed");

    //define server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //bind socket to address
    t = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    handleError(t < 0, "bind failed");

    //listen to socket
    t = listen(rfd, MAX_CLIENTS);
    handleError(t < 0, "listen failed");

    //message queue for subscribers
    int subQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    handleError(subQueue < 0, "msgget failed");

    //handle queue
    int qpid = 0;
    qpid = fork();
    handleError(qpid < 0, "fork failed");
    if (qpid == 0) {
        //child process
        printf("queue process %d created\n", getpid());
        handleQueue(subQueue, subscriberStore);
        exit(EXIT_SUCCESS);
    }

    while (!endProgram) {
        //create child process
        int pid = fork();
        handleError(pid < 0, "fork failed");

        if (pid == 0) {
            //child process
            int i = 0;
            while(!endProgram){
                if(i < MAX_CLIENTS) {
                    //accept incoming connections
                    len = sizeof(client);
                    cfd = accept(rfd, (struct sockaddr *) &client, &len);
                    handleError(cfd < 0, "accept failed");
                    i++;
                    pid = fork();
                    handleError(pid < 0, "fork failed");

                    if (pid == 0) {
                        //child process
                        handleClient(cfd, sharedArray, subscriberStore, subQueue);
                        i--;
                        close(cfd);
                        printf("client process %d terminated\n", getpid());
                        exit(EXIT_SUCCESS);
                    }else{
                        close(cfd);
                    }
                }
            }
        } else {
            //parent process
            while(!endProgram){}
            kill(0, SIGTERM);
            while (wait(NULL) > 0);
        }

        //close socket
        //close(clientQueue);

        //delete shared memory
        deleteSharedMemory(sharedArrayID);
        deleteSharedMemory(subscriberStoreID);
        deleteSharedMemory(blockerID);

        deleteSemaphores(mutex);
        deleteSemaphores(block);
    }

    //close socket
    close(rfd);
}
