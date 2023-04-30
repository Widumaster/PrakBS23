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

#include "inputHandler.h"
#include "common.h"
#include "Semaphore.h"
#include "cmdEnum.h"
#include "subStore.h"
#include "subStore.h"

#define TRUE 1
#define PORT 5678
#define MAX_CLIENTS 50
#define SEGSIZE sizeof(Message) * ARRSIZE

semaphore mutex;
semaphore block;
semaphore subStoreMutex;

int *blocker = 0;


void handleError(int bool, char *msg) {
    if (bool) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void notify(SubscriberStore *subscriberStore, SubMessage subMessage) {
    for (int i = 0; i < MAXKEYS; i++) {
        if (strcmp(subscriberStore[i].key, subMessage.key) == 0) {
            char msg[BUFSIZE] = "";
            strcpy(msg, subMessage.value);
            snprintf(msg, BUFSIZE, "%s:%s:%s\n", getCmdString(subMessage.cmd), subMessage.key, subMessage.value);
            for (int j = 0; j < MAXSUBS; j++) {
                if (subscriberStore[i].pids[j] == 0)
                    return;
                printf("Sending to %d: %s\n", subscriberStore[i].pids[j], msg);
                write(subscriberStore[i].pids[j], msg, BUFSIZE);
            }
            return;
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

    while (TRUE) {
        if (msgrcv(qid, &subMessage, sizeof(SubMessage), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Received: %s\n", subMessage.key);

        if(strcmp(subMessage.key, "") != 0){
            down(subStoreMutex, 0);
            notify(subscriberStore, subMessage);
            up(subStoreMutex, 0);
        }

    }
}

void handleClient(int cfd, Message *messages, SubscriberStore *subscriberStore, int qid) {
    char in[BUFSIZE]; // Client Data
    int bytes_read; // number read, -1 for errors or 0 for EOF

    memset(in, 0, BUFSIZE);
    bytes_read = read(cfd, in, BUFSIZE);

    int action = -1;

    while (bytes_read > 0) {
        // wait until the client is blocker or no one is blocker
        while (*blocker != 0 && *blocker != getpid()) {
            sleep(1);
        }

        down(mutex, 0);
        action = handleInput(messages, subscriberStore, in, cfd, qid);
        up(mutex, 0);
        printf("action: %i\n", action);

        write(cfd, in, sizeof(in));

        switch (action) {
            case QUIT:
                close(cfd);
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
                .pids = {0}
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
    int msgQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    handleError(msgQueue < 0, "msgget failed");

    //handle queue
    int qpid = 0;
    qpid = fork();
    handleError(qpid < 0, "fork failed");
    if (qpid == 0) {
        //child process
        handleQueue(msgQueue, subscriberStore);
        exit(EXIT_SUCCESS);
    }

    while (TRUE) {
        //create child process
        int pid = fork();
        handleError(pid < 0, "fork failed");

        int i = 0;
        if (pid == 0) {
            while(TRUE){
                if(i < MAX_CLIENTS) {
                    i++;
                    pid = fork();
                    handleError(pid < 0, "fork failed");

                    if (pid == 0) {
                        //accept incoming connections
                        len = sizeof(client);
                        cfd = accept(rfd, (struct sockaddr *) &client, &len);
                        handleError(cfd < 0, "accept failed");

                        //child process
                        handleClient(cfd, sharedArray, subscriberStore, msgQueue);
                        i--;
                        close(cfd);
                        kill(getpid(), SIGINT);
                        //exit(EXIT_SUCCESS);
                    }
                } else {
                    //kill(getpid(), SIGINT);
                }
            }
        } else {
            //parent process
            while(TRUE){

            }
        }

        //close socket
        close(cfd);

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
