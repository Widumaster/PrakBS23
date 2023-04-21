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
#include <semaphore.h>

#include "inputHandler.h"
#include "dynArray.h"

#define BUFSIZE 1024 // Größe des Buffers
#define TRUE 1
#define PORT 4711
#define MAX_CLIENTS 5
#define ARRAY_INIT_SIZE 128
#define SEGSIZE sizeof(Message) * ARRAY_INIT_SIZE + sizeof(size_t) * 2

void Process(int cfd, struct sockaddr_in server, Array *messages){
    char in[BUFSIZE]; // Client Data
    int bytes_read; // number read, -1 for errors or 0 for EOF.

    // fill n with \000
    memset(in, 0, BUFSIZE);
    // Lesen von Daten, die der Client schickt
    bytes_read = read(cfd, in, BUFSIZE);

    printf("Client Message: %s", in);

    // Process data and send Answer back to client
    while (bytes_read > 0) {
        handleInput(messages, in);

        write(cfd, in, sizeof(in));
        memset(in, 0, BUFSIZE);

        bytes_read = read(cfd, in, BUFSIZE);
    }
}

int createSHM(){
    int id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
    if(id < 0){
        fprintf(stderr,"Creating shm failed");
        exit(1);
    }
    return id;
}
int fillSHM(int id, Array sharedArray){
    Array *share_mem;
    share_mem = (Array *)shmat(id, 0, 0);


}

void Server(){
    int pid[MAX_CLIENTS];
    int id;//, *shar_mem;
    printf("SEGSIZE: %i", SEGSIZE);
    Array *share_mem;

    id = createSHM();

    share_mem = (Array *)shmat(id, 0, 0);
    if (share_mem == (Array *)-1) {
        fprintf(stderr,"shmat failed");
        exit(1);
    }

    initArray(share_mem, ARRAY_INIT_SIZE);

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
            // Child process
            i++;
            close(rfd);
            Process(cfd, server, share_mem);
            exit(0);
        } else {
            // Parent process
            i++;
            Process(cfd, server, share_mem);
            //close(cfd);
        }

    }


    // Rendevouz Descriptor schließen
    close(rfd);
}