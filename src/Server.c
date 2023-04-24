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

#include "inputHandler.h"

#define TRUE 1
#define PORT 5678
#define MAX_CLIENTS 50
#define SEGSIZE sizeof(Message) * ARRSIZE

void clientProcess(int cfd, struct sockaddr_in server, Message *messages, int sem_id){
    char in[BUFSIZE]; // Client Data
    int bytes_read; // number read, -1 for errors or 0 for EOF.

    struct sembuf enter, leave; // Structs für den Semaphor
    enter.sem_num = leave.sem_num = 0;  // Semaphor 0 in der Gruppe
    enter.sem_flg = leave.sem_flg = SEM_UNDO;
    enter.sem_op = -1; // blockieren, DOWN-Operation
    leave.sem_op = 1;   // freigeben, UP-Operation

    // fill n with \000
    memset(in, 0, BUFSIZE);
    // Lesen von Daten, die der Client schickt
    bytes_read = read(cfd, in, BUFSIZE);

    printf("Client Message: %s", in);

    // Process data and send Answer back to client
    while (bytes_read > 0) {
        semop(sem_id, &enter, 1);
        int quit = handleInput(messages, in);
        semop(sem_id, &leave, 1);

        write(cfd, in, sizeof(in));
        memset(in, 0, BUFSIZE);

        if(quit == 1){
            close(cfd);
            return;
        }

        bytes_read = read(cfd, in, BUFSIZE);
    }
}

void Server(){
    int pid[MAX_CLIENTS];
    int shm_id, sem_id;
    unsigned short marker[1];

    Message *share_mem;

    sem_id = semget (IPC_PRIVATE, 1, IPC_CREAT|0644);
    if (sem_id == -1) {
        perror ("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

    marker[0] = 1;
    semctl(sem_id, 1, SETALL, marker);  // alle Semaphore auf 1

    shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);

    share_mem = (Message *)shmat(shm_id, 0, 0);
    if (share_mem == (Message *)-1) {
        fprintf(stderr,"shmat failed");
        exit(1);
    }

    for (int i = 0; i < ARRSIZE; ++i) {
        Message message = {
                .key = "",
                .value = "",
                .deleted = 0,
        };

        share_mem[i] = message;
    }

    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten

    pid_t children_pid[MAX_CLIENTS];

    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }

    // Socket lauschen lassen
    int lrt = listen(rfd, MAX_CLIENTS);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    int i = 0;
    while (TRUE) {
        children_pid[i] = fork();
        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        if(children_pid[i] == -1){
            printf("Kindprozess konnte nicht erzeugt werden!\n");
            exit(1);
        }

        if( i >= MAX_CLIENTS){
            fprintf("stderr", "Max clients reached");
            close(cfd);
            continue;
        }

        if (children_pid[i] < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (children_pid[i] == 0) {
            int count = 0;

            // Child process
            close(rfd);
            i++;
            clientProcess(cfd, server, share_mem, sem_id);
            exit(0);
        } else {
            // Parent process
            i++;
            clientProcess(cfd, server, share_mem, sem_id);
        }
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID);
    }


    // Rendevouz Descriptor schließen
    close(rfd);
}