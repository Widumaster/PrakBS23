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

#include "inputHandler.h"
#include "dynArray.h"

#define BUFSIZE 1024 // Größe des Buffers
#define TRUE 1
#define PORT 4711
#define MAX_CLIENTS 5
#define SEGSIZE sizeof(Message)

void Process(int cfd, struct sockaddr_in server, Array *messages){
    char in[BUFSIZE]; // Daten vom Client an den Server
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat

    // Lesen von Daten, die der Client schickt
    memset(in, 0, BUFSIZE);
    bytes_read = read(cfd, in, BUFSIZE);

    printf("Client Message: %s", in);

    // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
    while (bytes_read > 0) {
        printf("sending back the %d bytes I received...\n", bytes_read);
        handleInput(&messages, in);

        write(cfd, in, sizeof(in));
        memset(in, 0, BUFSIZE);

        bytes_read = read(cfd, in, BUFSIZE);
    }
}

void Server(){
    Array messages;
    initArray(&messages,128);

    int pid[MAX_CLIENTS];
    int id, *shar_mem;

    id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
    shar_mem = (int *)shmat(id, 0, 0);
    *shar_mem = 0;

    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten


    pid_t child_pid;


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

    while (TRUE) {

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);

        child_pid = fork();

        if (child_pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            // Child process
            close(rfd);
            Process(cfd, server, &messages);
            exit(0);
        } else {
            // Parent process
            Process(cfd, server, &messages);
            //close(cfd);
        }

    }

    // Rendevouz Descriptor schließen
    close(rfd);
}