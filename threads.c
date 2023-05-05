//
// Created by Widumaster on 26.04.2023.
//

#include "threads.h"
#include "requests.h"
#include <stdlib.h>


#include "sys/sem.h"
#include <sys/sem.h>      // Headerfile für Semaphore
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
//Vorbereitung für die Implementation von "pthreads" und "Semaphoren" um von Single thread zu Multithread zu transition

struct sembuf enter, leave;

pthreadInformation create_pthreadInformation(){

    pthreadInformation pthreadWrapp;
    pthreadWrapp.clientID = 0;
    return pthreadWrapp;
}

pthreadInformation createpthreadInformation(Server* server, CommandStruct commandStruct, int ClientID){

    pthreadInformation pthreadWrapp = create_pthreadInformation();

    pthreadWrapp.server = server;
    pthreadWrapp.commandStruct = commandStruct;
    pthreadWrapp.clientID = ClientID;

    return pthreadWrapp;
}

void threadRequestSwitch(pthreadInformation* pthreadWrapp){

    switch (pthreadWrapp->commandStruct.commandType) {
        case CommandPUT:
            requestPUT(pthreadWrapp);
            break;

        case CommandGET:
            requestGET(pthreadWrapp);
            break;

        case CommandDEL:
            requestDEL(pthreadWrapp);
            break;

        case CommandBEG:
            requestBEG(pthreadWrapp);
            break;

        case CommandEND:
            requestEND(pthreadWrapp);
            break;

        case CommandSUB:
            requestSUB(pthreadWrapp);
            break;

        case CommandQUIT:
            requestQUIT(pthreadWrapp);
    }

    free(pthreadWrapp);

}

void initializeSem(Server* server){

// Für die Semaphorgruppe und
// aus technischen Gründen eine Variable marker[1].
    unsigned short marker[1];

// Es folgt das Anlegen der Semaphorgruppe. Es wird hier nur ein
// Semaphor erzeugt
    server->SemaphoreID = semget (IPC_PRIVATE, 1, IPC_CREAT|0644);
    if (server->SemaphoreID == -1) {
        perror ("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

// Anschließend wird der Semaphor auf 1 gesetzt
    marker[0] = 1;
    semctl(server->SemaphoreID, 1, SETALL, marker);  // alle Semaphore auf 1


     // Structs für den Semaphor
    enter.sem_num = leave.sem_num = 0;  // Semaphor 0 in der Gruppe
    enter.sem_flg = leave.sem_flg = SEM_UNDO;
    enter.sem_op = -1; // blockieren, DOWN-Operation
    leave.sem_op = 1;   // freigeben, UP-Operation
}

void openSem(Server* server){
    semop(server->SemaphoreID, &enter, 1); // Eintritt in kritischen Bereich
}

void closeSem(Server* server){
    semop(server->SemaphoreID, &leave, 1); // Verlassen des krit. Bereichs
}

void clearSem(int sem_id){
    //Semaphorgruppe wird freigeschaltet
    semctl(sem_id, 0, IPC_RMID);
}
