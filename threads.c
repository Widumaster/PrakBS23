//
// Created by Widumaster on 26.04.2023.
//

#include "threads.h"
#include <pthread.h>
#include "requests.h"
#include <stdlib.h>
//Vorbereitung für die Implementation von "pthreads" und "Semaphoren" um von Single thread zu Multithread zu transition

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

