//
// Created by Widumaster on 26.04.2023.
//

#ifndef PRAKBS23_THREADS_H
#define PRAKBS23_THREADS_H

#include "socket.h"
#include "commandStruct.h"

typedef struct pthreadInformation_{

    Server* server;
    CommandStruct commandStruct;
    int clientID;

}pthreadInformation;

pthreadInformation createpthreadInformation(Server* server, CommandStruct commandStruct, int ClientID);
void threadRequestSwitch(pthreadInformation* pthreadWrapp);
pthreadInformation create_pthreadInformation();

void initializeSem(Server* server);
void openSem(Server* server);
void closeSem(Server* server);
void clearSem(int sem_id);

#endif //PRAKBS23_THREADS_H
